using UnityEngine;
using System.Collections;

public class LevelUiTest : MonoBehaviour {
	float lastTime;
	// Use this for initialization
	void Start () {
		lastTime = this.GetComponent<AudioSource> ().time;
	}
	
	// Update is called once per frame
	void Update () {
		if (this.GetComponent<AudioSource> ().time >= lastTime + 5.0f) {
			lastTime = this.GetComponent<AudioSource> ().time;
			Debug.Log (this.GetComponent<AudioSource> ().time);
		}
	}
}
