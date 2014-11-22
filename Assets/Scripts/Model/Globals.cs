using System.Collections;

public class Globals{

    public enum GameMode
    {
        MAINMENU, SONGSELECTION, LOADING, PLAYING, PAUSING, SCORESCREEN
    }

    public static GameMode Mode = GameMode.MAINMENU;

    public static string CurrentSongFolderPath = "C:/UnityProjekte/Drum Master/drum-master/Assets/Resources/Songs/35629 Bomfunk MC's - Freestyler";

}
