//Gracie Chaudhary D00251769  
//Dominik Hampejs D00250604  
#include "MusicPlayer.hpp"

MusicPlayer::MusicPlayer()
	: m_volume(10.f)
{
	m_filenames[MusicThemes::kMenuTheme] = "Media/Music/MenuTheme.ogg";
	m_filenames[MusicThemes::kMissionTheme] = "Media/Music/MissionTheme.ogg";

	//GracieChaudhary - uploading new music
	m_filenames[MusicThemes::kMenu] = "MediaFiles/Music/MenuMusic.ogg";
	m_filenames[MusicThemes::kGame] = "MediaFiles/Music/GameplayMusic.ogg";
	m_filenames[MusicThemes::kGameOver] = "MediaFiles/Music/GameOverMusic.wav";
}

void MusicPlayer::Play(MusicThemes theme)
{
	std::string filename = m_filenames[theme];

	if (!m_music.openFromFile(filename)) //streaming the music not loading
		throw std::runtime_error("Music " + filename + " could not be loaded.");

	m_music.setVolume(m_volume);
	m_music.setLoop(true);
	
	m_music.play();
}

void MusicPlayer::Stop()
{
	m_music.stop();
}

void MusicPlayer::SetVolume(float volume)
{
	m_volume = volume;
}

void MusicPlayer::SetPaused(bool paused)
{
	if (paused)
		m_music.pause();
	else
		m_music.play();
}