using UnityEditor;
using UnityEngine;

public class SoundImport : AssetPostprocessor
{

    public void OnPreprocessAudio()
    {
        AudioImporter ai = assetImporter as AudioImporter;
        ai.threeD = false;
        //Debug.Log("Bitrate: " + ai.compressionBitrate);
        ai.compressionBitrate = 300000;
        ai.loadType = AudioImporterLoadType.StreamFromDisc;
    }
}
