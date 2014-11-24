using UnityEngine;
using System.Collections;

public class HitObjectTrailBehaviour : MonoBehaviour {

	Vector2 DeltaSize;
	float DeltaAspect;
	RectTransform ProgressBar;
	float OldY;
	// Use this for initialization
	void Start () {
		//resize
		DeltaSize = this.GetComponent<RectTransform>().sizeDelta;
		DeltaAspect = Camera.main.aspect; 
		float ScaleX = Camera.main.pixelWidth / 1280;
		float ScaleY = Camera.main.pixelHeight / 720;
		this.GetComponent<RectTransform> ().sizeDelta = new Vector2 (DeltaSize.x * ScaleX,DeltaSize.y * ScaleY);
		//replace
		ProgressBar = GameObject.Find ("ProgressBar").GetComponent<RectTransform> ();
		OldY = ProgressBar.GetComponent<RectTransform> ().anchoredPosition.y;
	}
	
	// Update is called once per frame
	void Update () {
		if (DeltaAspect != Camera.main.aspect) {
			//resize
			float ScaleX = Camera.main.pixelWidth / 1280;
			float ScaleY = Camera.main.pixelHeight / 720;
			this.GetComponent<RectTransform> ().sizeDelta = new Vector2 (DeltaSize.x * ScaleX,DeltaSize.y * ScaleY);
			DeltaAspect = Camera.main.aspect;
		}
		if (ProgressBar.GetComponent<RectTransform> ().anchoredPosition.y != OldY) {
			//replace
			float ScaleX = Camera.main.pixelWidth / 1280;
			float ScaleY = Camera.main.pixelHeight / 720;
			float y = (ProgressBar.anchoredPosition.y - ProgressBar.sizeDelta.y) - 40 * ScaleY;
			y = this.name.Equals ("HitObjectTrailUp") ? y:y-=this.GetComponent<RectTransform>().sizeDelta.y;
			this.GetComponent<RectTransform> ().anchoredPosition = new Vector2 (0,y);
		}
	}
}