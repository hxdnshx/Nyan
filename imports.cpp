//--------------------------------------------------------------------------------------
// Copyright (c) No Need Name Dev Teams
// https://www.nnn-team.com/
//
// Desc : 导入库
//--------------------------------------------------------------------------------------

#include "../../../Src/common/common-macro.h"
#include "../../../Src/nnnEngine/core-macro.h"

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32) || (NNN_PLATFORM == NNN_PLATFORM_WP8)

#if (NNN_PLATFORM == NNN_PLATFORM_WP8)
#pragma comment(lib, "xaudio2.lib")
#endif	// NNN_PLATFORM_WP8

#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
#ifdef NNN_USE_OPENGLES
	#pragma comment(lib, "../../../3rdParty/OpenGLES_Emu/lib/libEGL.lib")
	#pragma comment(lib, "../../../3rdParty/OpenGLES_Emu/lib/libGLESv2.lib")
#else
	#pragma comment(lib, "opengl32.lib")
	#pragma comment(lib, "glu32.lib")
	#pragma comment(lib, "glew.lib")
#endif	// NNN_USE_OPENGLES
#endif	// NNN_PLATFORM_WIN32

#pragma comment(lib, "nnnEngine.lib")
#pragma comment(lib, "nnnLib.lib")
#pragma comment(lib, "../../../3rdParty/nnnPacket/libs/"PLATFORM_DIR"/"CONFIGURATION_DIR"/nnnPacket.lib")

#if (NNN_PLATFORM == NNN_PLATFORM_WP8)
#pragma comment(lib, "nnnWP8.lib")
#pragma comment(lib, "nnnWP8Entry.lib")
#endif	// NNN_PLATFORM_WP8

#pragma comment(lib, "7zip.lib")
#pragma comment(lib, "Cryptopp.lib")
#pragma comment(lib, "FreeType.lib")
#pragma comment(lib, "giflib.lib")
#pragma comment(lib, "jasper.lib")
#pragma comment(lib, "LibJPEG.lib")
#pragma comment(lib, "LibPNG.lib")
#pragma comment(lib, "ogg.lib")
#if (NNN_PLATFORM == NNN_PLATFORM_WIN32)
#pragma comment(lib, "OpenAL.lib")
#endif	// NNN_PLATFORM_WIN32
#pragma comment(lib, "OpenCV_Core.lib")
#pragma comment(lib, "OpenCV_ImgProc.lib")
#pragma comment(lib, "OpenCV_HighGUI.lib")
#pragma comment(lib, "TinyXML.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "zlib.lib")

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libavutil.lib")
#pragma comment(lib, "libavcodec.lib")
#pragma comment(lib, "libavformat.lib")
#pragma comment(lib, "libswresample.lib")

#endif	// NNN_PLATFORM_WIN32 || NNN_PLATFORM_WP8
