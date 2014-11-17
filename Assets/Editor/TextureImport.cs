using UnityEngine;
 using UnityEditor;
 
 class TextureImport : AssetPostprocessor {
     void OnPreprocessTexture () {
         TextureImporter textureImporter = assetImporter as TextureImporter;
         //textureImporter.compressionQuality = (int)TextureCompressionQuality.Best;
         //textureImporter.textureFormat = TextureImporterFormat.RGB24;
         textureImporter.isReadable = true;
     }
 }
