using UnityEngine;
using System.Collections;
using Assets.Scripts;
using System.Diagnostics;
using UnityEditor;

public class SomeScript : MonoBehaviour {

    public string pathToOsuFile = "";
    public AudioClip tickSound;
    private int counter = 1;
    private System.Random random = new System.Random();
    private Stopwatch stopwatch = new Stopwatch();
    //int flash = 0;
    int relativeTime = 70;
    int preLoadTime = 2000;
    bool relativelyTimed = false;
    double absoluteStartTime;

    private SongObject songObject = null;

    private TimingPoint activeTimingPoint = null;

    private int nextBar = 100000;

    private int minTimeSpacing = 180;

	// Use this for initialization
	void Start () {
        stopwatch.Start();
        this.songObject = LevelBuilder.ImportSongs()[0];
        UnityEngine.Debug.Log("music path:" + songObject.AudioFilePath);
        this.audio.clip = (AudioClip)Instantiate(Resources.Load(songObject.AudioFilePath.Substring(0, songObject.AudioFilePath.Length -4), typeof(AudioClip)));
        this.guiTexture.texture = (Texture)Instantiate(Resources.Load(songObject.ImageFilePath.Substring(0, songObject.ImageFilePath.Length - 4), typeof(Texture)));
        absoluteStartTime = AudioSettings.dspTime + preLoadTime / 1000.0d;
        audio.PlayScheduled(absoluteStartTime);
        
	}
	
	// Update is called once per frame
	void Update () {
        // timing & synchronisation
        if (!relativelyTimed && AudioSettings.dspTime >= (absoluteStartTime - ((1 + songObject.Offset) / 1000.0)))
        {

            relativeTime += (int)(Time.time * 1000.0);
            relativelyTimed = true;
            UnityEngine.Debug.Log(relativeTime);
        }
        /* if (flash > 0)
        {
            flash--;
        }*
         */
        int now = (int)(Time.time * 1000.0) - relativeTime;

        // Zeitangaben mit:
        //audio.time

        // update timing point
        //UnityEngine.Debug.Log(songObject.GetTimingPoints());
        if (activeTimingPoint == null || (songObject.GetTimingPoints()[0].Time <= now))
        {
            activeTimingPoint = songObject.GetTimingPoints()[0];
            songObject.GetTimingPoints().RemoveAt(0);
            UnityEngine.Debug.Log("new timing point!");
            activeTimingPoint.Debug();
        }

        HitObject hitObject = songObject.GetHitObjects()[0];
        if (now >= hitObject.Time)
        {
            songObject.GetHitObjects().RemoveAt(0);
            //this.camera.backgroundColor = Color.white;
            counter++;
            //flash = 10;
            audio.PlayOneShot(tickSound, 0.5f);

            // is next hit object too close? remove it.
            // easy mode
            if (hitObject.Time + minTimeSpacing > songObject.GetHitObjects()[0].Time)
            {
                songObject.GetHitObjects().RemoveAt(0);
                UnityEngine.Debug.Log("deleted hitsound!!!");
            }

            // if hitObject is a slider
            if (hitObject.IsSlider())
            {
                // calculate time of additional hitobjects
                int timeToNextObject = (int)((hitObject.SliderLength * 600f) / (activeTimingPoint.BPM * activeTimingPoint.Multiplier * songObject.SliderVelocity));
                UnityEngine.Debug.Log("time to next object: " + timeToNextObject + ", "+ hitObject.Time + ", " + hitObject.SliderObjects);
                for (int a = hitObject.SliderObjects; a > 0 ; a--)
                {
                    UnityEngine.Debug.Log("a: " + a);
                    HitObject newHitObject = new HitObject(hitObject.X, hitObject.Y, hitObject.Time + (a * timeToNextObject), 1, 1);
                    songObject.GetHitObjects().Insert(0, newHitObject);
                    UnityEngine.Debug.Log("insert additional hitobject at: " + newHitObject.Time);
                }
            }
        }
        else
        {
           // this.camera.backgroundColor = new Color(0.5f + 0.025f * flash, 0.5f + 0.025f * flash, 0.5f + 0.025f * flash);
        }
        //this.camera.backgroundColor = new Color(0.5f + 0.05f * flash, 0.5f + 0.05f * flash, 0.5f + 0.05f * flash);
	}

    private void CreateBarsFromBPM(TimingPoint timingPoint)
    {
        float bpm = timingPoint.BPM;
    }
}
