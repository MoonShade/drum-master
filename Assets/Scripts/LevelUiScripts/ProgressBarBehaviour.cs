using UnityEngine;
using System.Collections;

public class ProgressBarBehaviour : MonoBehaviour {
	// Use this for initialization
	public AudioClip AudioSource;
	Vector2 DeltaSize;
	float DeltaAspect;

	//TestingStuff
	float Timer;
	void Start () {
		// Audiosource willbe determinated by the song which should be played/is playing at this level
		// AudioSource = ...

		//resize
		DeltaSize = this.GetComponent<RectTransform>().sizeDelta;
		DeltaAspect = Camera.main.aspect; 
		float ScaleX = Camera.main.pixelWidth / 1280;
		float ScaleY = Camera.main.pixelHeight / 720;
		this.GetComponent<RectTransform> ().sizeDelta = new Vector2 (DeltaSize.x * ScaleX,DeltaSize.y * ScaleY);
		this.GetComponent<RectTransform> ().anchoredPosition = new Vector2 (40 * ScaleX, -40 * ScaleY);
		//testing Variables etc.
		Timer = Time.deltaTime;
	}
	
	// Update is called once per frame
	void Update () {
		//TestingStuff
		if (Timer != Time.deltaTime) {
			this.GetComponent<UnityEngine.UI.Image>().fillAmount = this.GetComponent<UnityEngine.UI.Image>().fillAmount >= 1 ? 0 : this.GetComponent<UnityEngine.UI.Image>().fillAmount + 0.001f;
		}
		//Resizing
		if (DeltaAspect != Camera.main.aspect) {
			float ScaleX = Camera.main.pixelWidth / 1280;
			float ScaleY = Camera.main.pixelHeight / 720;
			//resize
			this.GetComponent<RectTransform> ().sizeDelta = new Vector2 (DeltaSize.x * ScaleX,DeltaSize.y * ScaleY);
			this.GetComponent<RectTransform> ().anchoredPosition = new Vector2 (40 * ScaleX, -40 * ScaleY);
			DeltaAspect = Camera.main.aspect;
		}
	}
}
