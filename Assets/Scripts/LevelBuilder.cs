using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Assets.Scripts;
using System.IO;
using System;
using Assets.Scripts.Model;
using Assets.Scripts.Parser;

public class LevelBuilder : MonoBehaviour {

    private static string songImportFolder;

	// Use this for initialization
	void Start () {
        songImportFolder = Application.dataPath + "/Resources/Songs";
        Debug.Log(songImportFolder);
        ImportSongs();
	}

    /// <summary>
    /// Import all songs from the song folder
    /// </summary>
    /// <returns></returns>
    public static List<SongObject> ImportSongs()
    {
        songImportFolder = Application.dataPath + "/Resources/Songs";
        Debug.Log(songImportFolder);
        List<SongObject> songList = new List<SongObject>();
        DirectoryInfo importDir = new DirectoryInfo(songImportFolder);
        DirectoryInfo[] songsDirs = importDir.GetDirectories();
        foreach (DirectoryInfo songDir in songsDirs)
        {
            //Debug.Log(songDir.Name);
            
            songList.Add(ImportSongFromDir(songDir));
            //Debug.Log("finished");
            return songList;
        }
        return null;
    }

    private static SongObject ImportSongFromDir(DirectoryInfo dir)
    {
        FileInfo[] files = dir.GetFiles();
        foreach (FileInfo file in files)
        {
            if (file.Extension.Equals(".osu"))
            {
                //Debug.Log(2);
                return ImportSongFromOsuFile(dir, file);
            }
        }
        return null;
    }

    private enum OsuFileSection { None, General, Editor, Metadata, Difficulty, Events, TimingPoints, Colours, HitObjects }

    private static SongObject ImportSongFromOsuFile(DirectoryInfo baseDir, FileInfo osuFile)
    {
        //try
        //{
            SongObject song = new SongObject();
            OsuFileSection section = OsuFileSection.None;
            float lastBpm = 0;
            bool bgPictureSet = false;

            string[] lines = System.IO.File.ReadAllLines(osuFile.FullName);
            foreach (string lineRaw in lines)
            {
                string line = lineRaw.Trim();
                //Debug.Log(lineRaw);

                if (line != null)
                {
                    if (line.Equals(""))
                    {
                        section = OsuFileSection.None;
                    }
                    else if (line.StartsWith("[") && line.EndsWith("]"))
                    {
                        string sectionName = line.Substring(1, line.Length - 2);
                        //Debug.Log("sectionName:" + sectionName);
                        section = (OsuFileSection)System.Enum.Parse(OsuFileSection.General.GetType(), sectionName, true);
                    }
                    else if (section == OsuFileSection.General)
                    {
                        string[] lineParts = SplitAndTrim(line, ':');
                        if (lineParts[0].Equals("AudioFilename"))
                        {
                            song.AudioFilePath = "Songs/" + baseDir.Name + "/" + lineParts[1];
                        }
                        else if (lineParts[0].Equals("Offset"))
                        {
                            song.Offset = int.Parse(lineParts[1]);
                        }
                    }
                    else if (section == OsuFileSection.Difficulty)
                    {
                        string[] lineParts = SplitAndTrim(line, ':');
                        if (lineParts[0].Equals("SliderMultiplier"))
                        {
                            song.SliderVelocity = float.Parse(lineParts[1]);
                        }
                    }
                    else if (section == OsuFileSection.Events)
                    {
                        if (line.StartsWith("0,0,\"") && !bgPictureSet)
                        {
                            string[] lineParts = SplitAndTrim(line, ',');
                            song.ImageFilePath = "Songs/" + baseDir.Name + "/" + lineParts[2].Substring(1, lineParts[2].Length - 2);
                            bgPictureSet = true;
                        }
                    }
                    else if (section == OsuFileSection.TimingPoints)
                    {
                        /* 145,344.827586206897,4,2,1,70,1,0
                         *  time,
                         *  60000 / value = bpm OR -(inherit)% of last BPM,
                         *  anzahl "viertel" - 4 = 4/4 takt,
                         *  art hitsounds - 1 = normal, 2 = soft, 3 = drums
                         *  hitsounds - 0 = default - 1 = custom1...,
                         *  volume 0-100,
                         *  1 = real timing point, 0 = inherit bpm etc.,
                         *  1 = kiai time (chorus), 0 = no kiai time
                         */
                        string[] lineParts = line.Split(',');


                        TimingPoint timingPoint = new TimingPoint();
                        timingPoint.Time = (int)float.Parse(lineParts[0]);
                        timingPoint.IsInherit = !ParseToBool(lineParts[6]);
                        //float bpm, multiplier = 100;
                        if (timingPoint.IsInherit)
                        {
                            timingPoint.BPM = lastBpm;
                            timingPoint.Multiplier = 100f / float.Parse(lineParts[1].Substring(1, lineParts[1].Length - 1));
                        }
                        else
                        {
                            timingPoint.BPM = 60000 / float.Parse(lineParts[1]);
                            lastBpm = timingPoint.BPM;
                            timingPoint.Multiplier = 1f;
                        }
                        timingPoint.Quarters = int.Parse(lineParts[2]);
                        timingPoint.HType = (HitsoundType)System.Enum.Parse(HitsoundType.Normal.GetType(), lineParts[3], true);
                        timingPoint.Volume = int.Parse(lineParts[5]);
                        timingPoint.IsChorus = ParseToBool(lineParts[7]);
                        //timingPoint.Debug();
                        song.AddTimingPoint(timingPoint);
                    }

                    else if (section == OsuFileSection.HitObjects)
                    {
                        //break;
                        string[] lineParts = line.Split(',');
                        //x, y, time in ms, type, hitsound, unknown
                        /* TYPE:
                         * Normal = 1,
                         * Slider = 2,
                         * NewCombo = 4,
                         * NormalNewCombo = 5,
                         * SliderNewCombo = 6,
                         * Spinner = 8,
                         * ColourHax = 112,
                         * Hold = 128,
                         * ManiaLong = 128
                         */
                        /* HITSOUND:
                         * None = 0,
                         * Normal = 1,
                         * Whistle = 2,
                         * Finish = 4,
                         * Clap = 8
                         */
                        int x = int.Parse(lineParts[0]);
                        int y = int.Parse(lineParts[1]);
                        int time = int.Parse(lineParts[2]);
                        int type = int.Parse(lineParts[3]);
                        int hitsound = int.Parse(lineParts[4]);

                        RawHitObject hitObject = new RawHitObject(x, y, time, type, hitsound);
                        song.AddRawHitObject(hitObject);
                        //Debug.Log("added hitboject");

                        if (type == 2 || type == 6)
                        {
                            // slider, add slider length
                            /*  Typ-Slider(B oder C, irrelevant für uns) |
                             *  Slider-Punkte mit x:y getrennt durch '|' ,
                             *  Anzahl 'Läufe' (1:Einfach, 2:1 Repeat, 3:2Repeats etc.) ,
                             *  Zeitlicher Abstand zum End-Circle ,
                             *  Hitsounds für alle Hitobjects, mindestens 2 getrennt durch '|',
                             *  unbekannt
                             */
                            /* lineParts[5] - Infos für Position und Typ des Sliders
                             * lineParts[6] - Anzahl Repeats
                             * lineParts[7] - 'räumliche' länge des sliders
                             * lineParts[8] - Hitsounds aller Hitobjects
                             */
                            hitObject.SliderLength = float.Parse(lineParts[7]);
                            hitObject.SliderObjects = int.Parse(lineParts[6]);
                            
                            /*int repeats = int.Parse(lineParts[6]);
                            for (int a = 0; a < repeats; a++)
                            {
                                time += (int)(float.Parse(lineParts[7]) * 3.2);
                                hitObject = new RawHitObject(x, y, time, type, hitsound);
                                song.AddHitObject(hitObject);
                                Debug.Log("added hitboject");
                            }*/

                        }

                    }
                }
            }
            //Debug.Log("out of loop");
            return song;
        //}
        //catch (Exception e)
        //{
            /*Debug.Log("The osu file with the path '" + path + "' could not be read:");
            Debug.Log(e.Message);*/
        //    throw e;
        //}

        //return null;
    }


    private static string[] SplitAndTrim(string line, char seperator)
    {
        string[] lineParts = line.Split(seperator);
        for (int a = 0; a < lineParts.Length; a++)
        {
            lineParts[a] = lineParts[a].Trim();
        }
        return lineParts;
    }

    private static bool ParseToBool(string s)
    {
        if (s.Trim().Equals("1"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

	
	// Update is called once per frame
	void Update () {
	
	}
}
