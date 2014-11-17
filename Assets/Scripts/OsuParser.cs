using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Scripts
{
    class OsuParser
    {

        public static SongObject ParseFile(String path)
        {

            try
            {
                bool collectHitObjects = false;
                SongObject song = new SongObject();

                string[] lines = System.IO.File.ReadAllLines(path);
                foreach (string line in lines)
                {
                    string lineTrimmed = line.Trim();
                    Debug.Log(line);

                    if (lineTrimmed != null)
                    {
                        if (lineTrimmed.Equals(""))
                        {
                            collectHitObjects = false;
                        }
                        else if (lineTrimmed.Equals("[HitObjects]"))
                        {

                            collectHitObjects = true;
                        }
                        else if (collectHitObjects)
                        {
                            String[] lineParts = lineTrimmed.Split(',');
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

                            HitObject hitObject = new HitObject(x, y, time, type, hitsound);
                            song.AddHitObject(hitObject);

                            if (type == 2 || type == 6)
                            {
                                // slider, add additional hitobjects
                                /*  Typ-Slider(B oder C, irrelevant für uns) |
                                 *  Slider-Punkte mit x:y getrennt durch '|' ,
                                 *  Anzahl 'Läufe' (1:Einfach, 2:1 Repeat, 3:2Repeats etc.) ,
                                 *  Zeitlicher Abstand zum End-Circle ,
                                 *  Hitsounds für alle Hitobjects, mindestens 2 getrennt durch '|',
                                 *  unbekannt
                                 */
                                /* lineParts[5] - Infos für Position und Typ des Sliders
                                 * lineParts[6] - Anzahl Repeats
                                 * lineParts[7] - Zeit zwischen Anfangs- und End-HitObject in ms
                                 * lineParts[8] - Hitsounds aller Hitobjects
                                 */
                                int repeats = int.Parse(lineParts[6]);
                                for (int a = 0; a < repeats; a++)
                                {
                                    time += (int)(float.Parse(lineParts[7]) * 3.2);
                                    hitObject = new HitObject(x, y, time, type, hitsound);
                                    song.AddHitObject(hitObject);
                                }

                            }

                        }
                    }
                }
                return song;
            }
            catch (Exception e)
            {
                /*Debug.Log("The osu file with the path '" + path + "' could not be read:");
                Debug.Log(e.Message);*/
                throw e;
            }

            return null;
        }


    }
}
