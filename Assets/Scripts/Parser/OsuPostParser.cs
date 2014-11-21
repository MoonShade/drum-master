using Assets.Scripts.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Assets.Scripts.Parser
{
    class OsuPostParser
    {

        public static SongObject RunPostParser(SongObject song)
        {
            // copy timing points and hit objects into new list
            List<TimingPoint> timingPoints = new List<TimingPoint>(song.GetTimingPoints());
            List<RawHitObject> rawHitObjects = new List<RawHitObject>(song.GetRawHitObjects());

            TimingPoint currentTimingPoint = timingPoints[0];
            timingPoints.RemoveAt(0);
            foreach(RawHitObject rawHitObject in rawHitObjects) {
                int time = rawHitObject.Time;
                if (timingPoints.Count > 0 && timingPoints[0] != null && timingPoints[0].Time <= time) {
                    // new timing point
                    currentTimingPoint = timingPoints[0];
                    timingPoints.RemoveAt(0);
                }

                // create apropriate hitobject from raw hitobject
                HitObject.TriggerType triggerType;
                if (rawHitObject.IsClickedLeft()) {
                    triggerType = HitObject.TriggerType.LEFT;
                } else if (rawHitObject.IsClickedRight()) {
                    triggerType = HitObject.TriggerType.RIGHT;
                } else {
                    triggerType = HitObject.TriggerType.BOTH;
                }

                HitObject newHitObject = new HitObject(time, triggerType, currentTimingPoint.Volume, currentTimingPoint.HType);
                song.AddHitObject(newHitObject);

                // if the raw hitobject is a slider, add additional hitobjects
                if (rawHitObject.IsSlider())
                {
                    // calculate time of additional hitobjects
                    int timeToNextObject = (int)((rawHitObject.SliderLength * 600f) / (currentTimingPoint.BPM * currentTimingPoint.Multiplier * song.SliderVelocity));
                    UnityEngine.Debug.Log("time to next object: " + timeToNextObject + ", " + rawHitObject.Time + ", " + rawHitObject.SliderObjects);
                    for (int a = 1; a <= rawHitObject.SliderObjects; a++)
                    {
                        UnityEngine.Debug.Log("a: " + a);
                        int sliderObjectTime = time + (a * timeToNextObject);
                        if (timingPoints.Count > 0 && timingPoints[0] != null && timingPoints[0].Time <= sliderObjectTime)
                        {
                            // new timing point
                            currentTimingPoint = timingPoints[0];
                            timingPoints.RemoveAt(0);
                        }
                        HitObject sliderHitObject = new HitObject(sliderObjectTime, triggerType, currentTimingPoint.Volume, currentTimingPoint.HType);
                        song.AddHitObject(sliderHitObject);
                        UnityEngine.Debug.Log("insert additional hitobject at: " + sliderHitObject.Time);
                    }
                }

            }


            return song;
        }



    }
}
