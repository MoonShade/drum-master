using UnityEngine;
using System.Collections;

public class LevelUIBackgroundBehaviour : MonoBehaviour {
	float oldAspect;
	Vector2 DeltaSize;
	// Use this for initialization
	void Start () {
		oldAspect = Camera.main.aspect;
		DeltaSize = this.GetComponent<RectTransform> ().sizeDelta;
		float ScaleX = Camera.main.pixelWidth / 1280;
		float ScaleY = Camera.main.pixelHeight / 720;
		this.GetComponent<RectTransform> ().sizeDelta = new Vector2 (DeltaSize.x * ScaleX,DeltaSize.y * ScaleY);
	}
	
	// Update is called once per frame
	void Update () {
		if (oldAspect != Camera.main.aspect) {
			float ScaleX = Camera.main.pixelWidth / 1280;
			float ScaleY = Camera.main.pixelHeight / 720;
			//resize
			this.GetComponent<RectTransform> ().sizeDelta = new Vector2 (DeltaSize.x * ScaleX,DeltaSize.y * ScaleY);
			oldAspect = Camera.main.aspect;
		}
	}
}
