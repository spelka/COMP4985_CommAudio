/*---------------------------------------------------------------------------------------
--	SOURCE FILE:		audiplayer.cpp -   Audi player window for controlling audio.
--
--	PROGRAM:			Server.exe
--
--	FUNCTIONS:			Application
--						~Application
--						on_pushButton_clicked()
--						on_actionConfigure_triggered()
--						appendToLog
--						updatePlaylist
--
--	DATE:				Febuary 19 2015
--
--	DESIGNERS:			Filip Gutica & Auto-generated
--
--	PROGRAMMERS:		Filip Gutica & Auto-generated
--
--	NOTES:
---------------------------------------------------------------------------------------*/
#include "audioplayer.h"
#include "ui_audioplayer.h"

/*------------------------------------------------------------------------------
--	FUNCTION: AudioPlayer()
--
--	PURPOSE:		Constructor, initializes the Ui object containing al ui elemnets
--
--	DESIGNERS:		Auto-generated
--
--	PROGRAMMER:		Auto-generated
/*-----------------------------------------------------------------------------*/
AudioPlayer::AudioPlayer(QWidget *parent) : QDialog(parent), ui(new Ui::AudioPlayer)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this, QMediaPlayer::StreamPlayback);

    groupAddress = QHostAddress("234.5.6.7");

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::AnyIPv4, 7575);
    udpSocket->joinMulticastGroup(groupAddress);
    udpSocket->setReadBufferSize(AUDIO_BUFFSIZE);

    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));

    format.setChannelCount(2);
    format.setSampleRate(44100);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    audio = new QAudioOutput(format, this);
    audio->setBufferSize(AUDIO_BUFFSIZE);

    data = new QByteArray();

    /*** Connect the audio palyer to the progress slider so they are synchronized ***/
    connect(player, &QMediaPlayer::positionChanged, this, &AudioPlayer::on_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &AudioPlayer::on_durationChanged);


    ioDevice = audio->start();


}

/*------------------------------------------------------------------------------
--	FUNCTION: ~Application()
--
--	PURPOSE:		Destructor, cleans up the ui. Deletes the ui object
--
--	DESIGNERS:		Auto-generated
--
--	PROGRAMMER:		Auto-generated
/*-----------------------------------------------------------------------------*/
AudioPlayer::~AudioPlayer()
{
    udpSocket->close();
    delete player;
    delete ui;
}

/*------------------------------------------------------------------------------
--	FUNCTION: on_btnPlay_clicked()
--
--	PURPOSE:		When play button is played. Sets the media to the current song,
--                  sets the volume, then plays the media.
--
--	PARAMETERS:
--		void
--
--	DESIGNERS:		Filip Gutica
--
--	PROGRAMMER:		Filip Gutica
/*-----------------------------------------------------------------------------*/
void AudioPlayer::on_btnPlay_clicked()
{
    /*
    player->setMedia(QUrl::fromLocalFile(filePath));
    player->setVolume(100);
    player->play();
    */



}

/*------------------------------------------------------------------------------
--	FUNCTION: on_btnPause_clicked()
--
--	PURPOSE:		Pauses the media player when pause button pressed
--
--	PARAMETERS:
--		void
--
--	DESIGNERS:		Filip Gutica
--
--	PROGRAMMER:		Filip Gutica
/*-----------------------------------------------------------------------------*/
void AudioPlayer::on_btnPause_clicked()
{
    player->pause();
}

/*------------------------------------------------------------------------------
--	FUNCTION: on_btnPause_clicked()
--
--	PURPOSE:		resumes the media player when resume button pressed
--
--	PARAMETERS:
--		void
--
--	DESIGNERS:		Filip Gutica
--
--	PROGRAMMER:		Filip Gutica
/*-----------------------------------------------------------------------------*/
void AudioPlayer::on_btnResume_clicked()
{
    player->play();
}

/*------------------------------------------------------------------------------
--	FUNCTION: on_sliderProgress_sliderMoved()
--
--	PURPOSE:		sets the position of the player based on the slider position
--
--	PARAMETERS:
--		int position    Position of the slider
--
--	DESIGNERS:		Filip Gutica
--
--	PROGRAMMER:		Filip Gutica
/*-----------------------------------------------------------------------------*/
void AudioPlayer::on_sliderProgress_sliderMoved(int position)
{
    player->setPosition(position);
}

/*------------------------------------------------------------------------------
--	FUNCTION: on_sliderProgress_sliderMoved()
--
--	PURPOSE:		set the volume of the player
--
--	PARAMETERS:
--		int position    Position of the slider
--
--	DESIGNERS:		Filip Gutica
--
--	PROGRAMMER:		Filip Gutica
/*-----------------------------------------------------------------------------*/
void AudioPlayer::on_sliderVolume_sliderMoved(int position)
{
    player->setVolume(position);
}

/*------------------------------------------------------------------------------
--	FUNCTION: setAudio(QString)
--
--	PURPOSE:		Set the audio file
--
--	PARAMETERS:
--		QString file    Path to the audio file to be played
--
--	DESIGNERS:		Filip Gutica
--
--	PROGRAMMER:		Filip Gutica
/*-----------------------------------------------------------------------------*/
void AudioPlayer::setAudio(QString file)
{
    filePath = file;
    this->setWindowTitle(filePath);
}

/*------------------------------------------------------------------------------
--	FUNCTION: on_positionChanged(qint64)
--
--	PURPOSE:		update the position of the slider as the audio progresses
--
--	PARAMETERS:
--		qint64 position    position that the audio player is in the current song
--
--	DESIGNERS:		Filip Gutica
--
--	PROGRAMMER:		Filip Gutica
/*-----------------------------------------------------------------------------*/
void AudioPlayer::on_positionChanged(qint64 position)
{
    ui->sliderProgress->setValue(position);
}

/*------------------------------------------------------------------------------
--	FUNCTION: on_durationChanged(qint64)
--
--	PURPOSE:		update the position of the slider as the audio progresses
--
--	PARAMETERS:
--		qint64 position    position that the audio player is in the current song
--
--	DESIGNERS:		Filip Gutica
--
--	PROGRAMMER:		Filip Gutica
/*-----------------------------------------------------------------------------*/
void AudioPlayer::on_durationChanged(qint64 position)
{
    ui->sliderProgress->setMaximum(position);
}

void AudioPlayer::processPendingDatagrams()
{
    QByteArray datagram;

    player->setMedia(QMediaContent(), udpSocket);

   while (udpSocket->hasPendingDatagrams())
   {
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        data->append(datagram.data(), datagram.size());

        //qDebug() << datagram.data();

        writeData(datagram);

   }
}

void AudioPlayer::writeData(QByteArray d)
{
    ioDevice->write(d.data(), d.size());
}
