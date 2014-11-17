using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Assets.Scripts
{
    public class TimingPoint
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

        public int Time { get; set; }
        public float BPM { get; set; }
        public float Multiplier { get; set; }
        public bool IsInherit { get; set; }
        public bool IsChorus { get; set; }
        public int Volume { get; set; }
        public int Quarters { get; set; }

        public enum HitsoundType { Normal = 1, Soft = 2, Drum = 3}
        public HitsoundType HType { get; set; }


        public void Debug()
        {
            UnityEngine.Debug.Log("Timing Point (starts at " + Time + ", BPM:" + BPM + ", MP:" + Multiplier + ", volume:" + Volume + ", inherit:" + IsInherit + ", chorus:" + IsChorus + ")");
        }
    }
}
