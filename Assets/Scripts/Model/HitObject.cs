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

        // TODO: weiter machen ab hier

        public bool IsClickedLeft()
        {
            return (hitsound == 0 || hitsound == 1);
        }

        public bool IsClickedRight()
        {
            return !IsClickedLeft() && !IsClickedBoth();
        }

        public bool IsClickedBoth()
        {
            return (hitsound == 4 || hitsound == 6 || hitsound == 12);
        }

        public HitObject(int x, int y, int time, int type, int hitsound)
        {
            this.X = x;
            this.Y = y;
            this.Time = time;
            this.type = type;
            this.hitsound = hitsound;
        }

        public bool IsSlider()
        {
            return type == 2 || type == 6;
        }
    }
}
