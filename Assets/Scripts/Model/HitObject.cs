using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Assets.Scripts.Model
{
    public class HitObject
    {

        public int Time { get; set; }
        public enum TriggerType { LEFT, RIGHT, BOTH }
        public TriggerType Trigger { get; set; }
        public int Volume { get; set; }
        public HitsoundType HType { get; set; }

        // TODO: weiter machen ab hier

        public HitObject(int time, TriggerType triggerType, int volume, HitsoundType hitsoundType)
        {
            this.Time = time;
            this.Trigger = triggerType;
            this.Volume = volume;
            this.HType = hitsoundType;
        }

    }
}
