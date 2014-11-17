using System.Collections;

public class Globals{

    public enum GameMode
    {
        MAINMENU, SONGSELECTION, LOADING, PLAYING, PAUSING, SCORESCREEN
    }

    public static GameMode Mode = GameMode.MAINMENU;

    public static string CurrentSongFolderName = "";

}
