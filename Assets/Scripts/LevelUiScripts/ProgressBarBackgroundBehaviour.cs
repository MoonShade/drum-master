using UnityEngine;
using System.Collections;

public class ProgressBarBackgroundBehaviour : MonoBehaviour {
	Vector2 DeltaSize;
	float DeltaAspect;
	// Use this for initialization
	void Start () {
		//resize
		DeltaSize = this.GetComponent<RectTransform>().sizeDelta;
		DeltaAspect = Camera.main.aspect; 
		float ScaleX = Camera.main.pixelWidth / 1280;
		float ScaleY = Camera.main.pixelHeight / 720;
		this.GetComponent<RectTransform> ().sizeDelta = new Vector2 (DeltaSize.x * ScaleX,DeltaSize.y * ScaleY);
		this.GetComponent<RectTransform> ().anchoredPosition = new Vector2 (40 * ScaleX, -40 * ScaleY);
	}
	
	// Update is called once per frame
	void Update () {
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
