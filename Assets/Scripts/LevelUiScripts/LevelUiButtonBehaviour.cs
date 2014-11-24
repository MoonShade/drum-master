using UnityEngine;
using System.Collections;

public class LevelUiButtonBehaviour : MonoBehaviour {
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
		float x = this.name.Equals("LeftButton") ? 0f : Camera.main.pixelWidth-this.GetComponent<RectTransform> ().sizeDelta.x;
		float y = (Camera.main.pixelHeight - this.GetComponent<RectTransform> ().sizeDelta.y) * -1f;
		this.GetComponent<RectTransform> ().anchoredPosition = new Vector2 (x, y);
	}
	
	// Update is called once per frame
	void Update () {
		if (DeltaAspect != Camera.main.aspect) {
			float ScaleX = Camera.main.pixelWidth / 1280;
			float ScaleY = Camera.main.pixelHeight / 720;
			//resize
			this.GetComponent<RectTransform> ().sizeDelta = new Vector2 (DeltaSize.x * ScaleX,DeltaSize.y * ScaleY);
			float x = this.name.Equals("LeftButton") ? 0f : Camera.main.pixelWidth-this.GetComponent<RectTransform> ().sizeDelta.x;
			float y = (Camera.main.pixelHeight - this.GetComponent<RectTransform> ().sizeDelta.y) * -1f;
			this.GetComponent<RectTransform> ().anchoredPosition = new Vector2 (x, y);
			DeltaAspect = Camera.main.aspect;
		}
	}
}