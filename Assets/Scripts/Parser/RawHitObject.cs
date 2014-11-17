using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Assets.Scripts.Parser
{
	class RawHitObject
	{
        /* coordinates from the osu! file, ignore them for now. */
        public int X { get; set; }
        public int Y { get; set; }

        public int Time { get; set; }
        //public float SliderLength { get; set; }
        //public int SliderObjects { get; set; }
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
        private int type;

        /* HITSOUND:
         * None = 0,
         * Normal = 1,
         * Whistle = 2,
         * Finish = 4,
         * Clap = 8
         */
        private int hitsound;

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
