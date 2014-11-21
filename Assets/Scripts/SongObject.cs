using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Assets.Scripts
{
    public class SongObject
    {

        private List<HitObject> hitObjects = new List<HitObject>();
        private List<TimingPoint> timingPoints = new List<TimingPoint>();
        public string AudioFilePath { get; set; }
        public string ImageFilePath { get; set; }
        public string Title { get; set; }
        public string Artist { get; set; }
        public string Creator { get; set; }
        public string Version { get; set; }
        public string Source { get; set; }
        public string Difficulty { get; set; }
        public float SliderVelocity { get; set; }
        public int Offset { get; set; }

        public void AddHitObject(HitObject hitObject) {
            hitObjects.Add(hitObject);
        }

        public List<HitObject> GetHitObjects(){
            return hitObjects;
        }

        public void AddTimingPoint(TimingPoint timingPoint)
        {
            timingPoints.Add(timingPoint);
        }

        public List<TimingPoint> GetTimingPoints()
        {
            return timingPoints;
        }

    }
}
