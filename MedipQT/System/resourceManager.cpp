#include "stdafx.h"
#include "resourceManager.h"
#include "stringManager.h"
#include "FolderCompressor.h"
#include "windowManager.h"

#include "zlib.h"
#include "qpixmap.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

/*
Compress from file source to file dest until EOF on source.

zip() return:
Z_OK : on success,
Z_MEM_ERROR if memory could not be allocated for processing,
Z_STREAM_ERROR if an invalid compression level is supplied,
Z_VERSION_ERROR if the version of zlib.h and the version of the library linked do not match,
Z_ERRNO if there is an error reading or writing the files.
*/

int zip(FILE *source, FILE *dest, int level)
{
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return ret;

	/* compress until end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void)deflateEnd(&strm);
			return Z_ERRNO;
		}
		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		/* run deflate() on input until output buffer not full, finish
		compression if all of source has been read in */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */
			_ASSERTE(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = CHUNK - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)deflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);
		_ASSERTE(strm.avail_in == 0);     /* all input will be used */

										/* done when last data in file processed */
	} while (flush != Z_FINISH);
	_ASSERTE(ret == Z_STREAM_END);        /* stream will be complete */

										/* clean up and return */
	(void)deflateEnd(&strm);
	return Z_OK;
}


/*
Decompress from file source to file dest until stream ends or EOF.

unzip() returns:
Z_OK on success,
Z_MEM_ERROR if memory could not be allocated for processing,
Z_DATA_ERROR if the deflate data is invalid or incomplete,
Z_VERSION_ERROR if the version of zlib.h and the version of the library linked do not match,
Z_ERRNO if there is an error reading or writing the files.
*/

int unzip(FILE *source, FILE *dest)
{
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	/* decompress until deflate stream ends or end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			return Z_ERRNO;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			_ASSERTE(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}
			have = CHUNK - strm.avail_out;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)inflateEnd(&strm);
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* report a zlib or i/o error */
void zerr(int ret)
{
	fputs("zpipe: ", stderr);
	switch (ret) {
	case Z_ERRNO:
		if (ferror(stdin))
			fputs("error reading stdin\n", stderr);
		if (ferror(stdout))
			fputs("error writing stdout\n", stderr);
		break;
	case Z_STREAM_ERROR:
		fputs("invalid compression level\n", stderr);
		break;
	case Z_DATA_ERROR:
		fputs("invalid or incomplete deflate data\n", stderr);
		break;
	case Z_MEM_ERROR:
		fputs("out of memory\n", stderr);
		break;
	case Z_VERSION_ERROR:
		fputs("zlib version mismatch!\n", stderr);
	}
}

ResourceManager * ResourceManager::getSingleton()
{
	static ResourceManager instance;
	return &instance;
}

ResourceManager::ResourceManager()
{
	imageUnloadAxial = 0;
	imageUnloadCoronal = 0;
	imageUnloadSagittal = 0;
	imageUnloadSubView = 0;
	imageUnloadVolume = 0;
	imageWaterMark = 0;
	image3DCullingPlane = 0;
	image3DFontNotoSansRegular = 0;
	imageRight = 0;
	imageLeft = 0;
	imageAnterior = 0;
	imagePosterior = 0;
	imageAnteriorHori = 0;
	imagePosteriorHori = 0;
	imageSuperior = 0;
	imageInferior = 0;
}

ResourceManager::~ResourceManager()
{
	clear();

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
		dir.removeRecursively();
}
void ResourceManager::clear()
{
	SAFE_DELETE(imageUnloadAxial);
	SAFE_DELETE(imageUnloadCoronal);
	SAFE_DELETE(imageUnloadSagittal);
	SAFE_DELETE(imageUnloadSubView);
	SAFE_DELETE(imageUnloadVolume);
	SAFE_DELETE(imageWaterMark);
	SAFE_DELETE(image3DCullingPlane);
	SAFE_DELETE(image3DFontNotoSansRegular);
	SAFE_DELETE(imageRight);
	SAFE_DELETE(imageLeft);
	SAFE_DELETE(imageAnterior);
	SAFE_DELETE(imagePosterior);
	SAFE_DELETE(imageAnteriorHori);
	SAFE_DELETE(imagePosteriorHori);
	SAFE_DELETE(imageSuperior);
	SAFE_DELETE(imageInferior);

	for (auto m = imageMap.begin(); m != imageMap.end(); ++m)
	{
		SAFE_DELETE(m.value());
	}
	imageMap.clear();
}

QIcon ResourceManager::getIcon(RICON icon_type, int width, int height)
{
	if (imageMap.size() == 0)
	{
		return QIcon();
	}
	else
	{
		QMap<QString, QByteArray*>::iterator result;
		QPixmap pix;

		switch (icon_type)
		{
			case ICON_NON_DIFF:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_diff_leave.png");		
				break;
			}
			case ICON_DIFF_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_diff_hover.png");
				break;
			}
			case ICON_DIFF:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_diff_press.png");
				break;
			}
			case ICON_NON_UNION:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_union_leave.png");
				break;
			}
			case ICON_UNION_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_union_hover.png");
				break;
			}
			case ICON_UNION:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_union_press.png");
				break;
			}
			case ICON_NON_INTERSECT:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_intersect_leave.png");
				break;
			}
			case ICON_INTERSECT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_intersect_hover.png");
				break;
			}
			case ICON_INTERSECT:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_intersect_press.png");
				break;
			}
			case ICON_NON_MESH_SPHERE:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_sphere_leave.png");
				break;
			}
			case ICON_MESH_SPHERE:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_sphere_press_blue.png");
				break;
			}
			case ICON_MESH_SPHERE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_sphere_hover.png");
				break;
			}
			case ICON_NON_MESH_CYLINDER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_cylinder_leave.png");
				break;
			}
			case ICON_MESH_CYLINDER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_cylinder_press_blue.png");
				break;
			}
			case ICON_MESH_CYLINDER_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_cylinder_hover.png");
				break;
			}
			case ICON_NON_MESH_CUBE:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_cube_leave.png");
				break;
			}
			case ICON_MESH_CUBE:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_cube_press_blue.png");
				break;
			}
			case ICON_MESH_CUBE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_cube_hover.png");
				break;
			}
			case ICON_NON_MESH_ZERO:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_reset_zero_leave.png");
				break;
			}
			case ICON_MESH_ZERO:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_reset_zero_press.png");
				break;
			}
			case ICON_MESH_ZERO_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mesh_reset_zero_hover.png");
				break;
			}
			case ICON_NON_SPLIT_CURVE:
			{
				result = imageMap.find("/MEDIP_FreeDraw3DCuttingOff.png");
				break;
			}
			case ICON_SPLIT_CURVE:
			{
				result = imageMap.find("/MEDIP_FreeDraw3DCuttingOff_MouseDown.png");
				break;
			}
			case ICON_SPLIT_CURVE_HOVER:
			{
				result = imageMap.find("/MEDIP_FreeDraw3DCuttingOff_MouseOver.png");
				break;
			}
			case ICON_NON_3D_ANGLE_SPLIT:
			{
				result = imageMap.find("/MEDIP_Icon_Angle_Split.png");
				break;
			}
			case ICON_3D_ANGLE_SPLIT_DOWN:
			{
				result = imageMap.find("/MEDIP_Icon_Angle_Split_MouseDown.png");
				break;
			}
			case ICON_3D_ANGLE_SPLIT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Angle_Split_MouseOver.png");
				break;
			}
			case ICON_ANIMATION_PLAY_BA_PRESS:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B-B-A_MouseDown.png");
				break;
			}
			case ICON_ANIMATION_PLAY_BA_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B-B-A_MouseOver.png");
				break;
			}
			case ICON_ANIMATION_PLAY_AB_PRESS:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B-A-B_MouseDown.png");
				break;
			}
			case ICON_ANIMATION_PLAY_AB_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B-A-B_MouseOver.png");
				break;
			}
			case ICON_ANIMATION_SINGLE_PRESS:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B_MouseDown.png");
				break;
			}
			case ICON_ANIMATION_SINGLE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B_MouseOver.png");
				break;
			}
			case ICON_NON_2D_VIEW_ON_3D:
			{
				result = imageMap.find("/MEDIP_Icon_ACSClip.png");
				break;
			}
			case ICON_2D_VIEW_ON_3D:
			{
				result = imageMap.find("/MEDIP_Icon_ACSClip_MouseDown.png");
				break;
			}
			case ICON_2D_VIEW_ON_3D_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_ACSClip_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_FULL_SCREEN:
			{
				result = imageMap.find("/MEDIP_Icon_FullScreen.png");
				break;
			}
			case ICON_VIEW_FULL_SCREEN:
			{
				result = imageMap.find("/MEDIP_Icon_FullScreen_MouseDown.png");
				break;
			}
			case ICON_VIEW_FULL_SCREEN_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_FullScreen_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_2D_AXISES:
			{
				result = imageMap.find("/MEDIP_Icon_2dViewAxises.png");
				break;
			}
			case ICON_VIEW_2D_AXISES_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_2dViewAxises_MouseOver.png");
				break;
			}
			case ICON_VIEW_2D_AXISES:
			{
				result = imageMap.find("/MEDIP_Icon_2dViewAxises_MouseDown.png");
				break;
			}
			case ICON_ANNO_LENGTH:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Length_MouseDown.png");
				break;
			}
			case ICON_NON_ANNO_LENGTH:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Length.png");
				break;
			}
			case ICON_ANNO_LENGTH_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Length_MouseOver.png");
				break;
			}
			case ICON_ANNO_ARROW:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Arrow_MouseDown.png");
				break;
			}
			case ICON_NON_ANNO_ARROW:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Arrow.png");
				break;
			}
			case ICON_ANNO_ARROW_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Arrow_MouseOver.png");
				break;
			}
			case ICON_ANNO_RECT:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Rectangle_MouseDown.png");
				break;
			}
			case ICON_NON_ANNO_RECT:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Rectangle.png");
				break;
			}
			case ICON_ANNO_RECT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Rectangle_MouseOver.png");
				break;
			}
			case ICON_VIEW_OVALROI:
			{
				result = imageMap.find("/MEDIP_Icon_OvalDraw_MouseDown.png");
				break;
			}
			case ICON_VIEW_NON_OVALROI:
			{
				result = imageMap.find("/MEDIP_Icon_OvalDraw.png");
				break;
			}
			case ICON_VIEW_OVALROI_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_OvalDraw_MouseOver.png");
				break;
			}
			case ICON_ANNO_ANGLE:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Angle_MouseDown.png");
				break;
			}
			case ICON_NON_ANNO_ANGLE:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Angle.png");
				break;
			}
			case ICON_ANNO_ANGLE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Annotation-Angle_MouseOver.png");
				break;
			}
			case ICON_ANNO_PROF_LINE:
			{
				result = imageMap.find("/MEDIP_Icon_Profile-Line_MouseDown.png");
				break;
			}
			case ICON_NON_ANNO_PROF_LINE:
			{
				result = imageMap.find("/MEDIP_Icon_Profile-Line.png");
				break;
			}
			case ICON_ANNO_PROF_LINE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Profile-Line_MouseOver .png");
				break;
			}
			case ICON_NON_TAB:
			{
				result = imageMap.find("/MEDIP_Icon_ShowHideTab.png");
				break;
			}
			case ICON_TAB:
			{
				result = imageMap.find("/MEDIP_Icon_ShowHideTab_MouseDown.png");
				break;
			}
			case ICON_TAB_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_ShowHideTab_MouseOver.png");
				break;
			}
			case ICON_CROP_NON_OPEN:
			{
				result = imageMap.find("/MEDIP_Icon_OpenAfterCropping.png");
				break;
			}
			case ICON_CROP_OPEN:
			{
				result = imageMap.find("/MEDIP_Icon_OpenAfterCropping_MouseDown.png");
				break;
			}
			case ICON_CROP_OPEN_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_OpenAfterCropping_MouseOver.png");
				break;
			}
			case ICON_FILE_NON_SAVE:
			{
				result = imageMap.find("/MEDIP_Icon_Save.png");
				break;
			}
			case ICON_FILE_SAVE:
			{
				result = imageMap.find("/MEDIP_Icon_Save_MouseDown.png");
				break;
			}
			case ICON_FILE_SAVE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Save_MouseOver.png");
				break;
			}
			case ICON_LIST_MESH_VISIBLE:
			{
				result = imageMap.find("/MEDIP_Icon_MeshView.png");
				break;
			}
			case ICON_FILE_NON_SAVE_AS:
			{
				result = imageMap.find("/MEDIP_Icon_SaveAs.png");
				break;
			}
			case ICON_FILE_SAVE_AS:
			{
				result = imageMap.find("/MEDIP_Icon_SaveAs_MouseDown.png");
				break;
			}
			case ICON_FILE_SAVE_AS_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_SaveAs_MouseOver.png");
				break;
			}
			case ICON_FILE_NON_UPLOAD:
			{
				result = imageMap.find("/MEDIP_Icon_Upload.png");
				break;
			}
			case ICON_FILE_UPLOAD:
			{
				result = imageMap.find("/MEDIP_Icon_Upload_MouseDown.png");
				break;
			}
			case ICON_FILE_UPLOAD_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Upload_MouseOver.png");
				break;
			}
			case ICON_PACS_NON_UPLOAD:
			{
				result = imageMap.find("/MEDIP_Icon_Upload_PACS.png");
				break;
			}
			case ICON_PACS_UPLOAD:
			{
				result = imageMap.find("/MEDIP_Icon_Upload_PACS_MouseDown.png");
				break;
			}
			case ICON_PACS_UPLOAD_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Upload_PACS_MouseOver.png");
				break;
			}
			case ICON_NON_UNDO:
			{
				result = imageMap.find("/MEDIP_Icon_Undo_inactive.png");
				break;
			}
			case ICON_UNDO_PRESS:
			{
				result = imageMap.find("/MEDIP_Icon_Undo_MouseDown.png");
				break;
			}
			case ICON_UNDO:
			{
				result = imageMap.find("/MEDIP_Icon_Undo.png");
				break;
			}
			case ICON_UNDO_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Undo_MouseOver.png");
				break;
			}

			case ICON_NON_REDO:
			{
				result = imageMap.find("/MEDIP_Icon_Redo_inactive.png");
				break;
			}
			case ICON_REDO:
			{
				result = imageMap.find("/MEDIP_Icon_Redo.png");
				break;
			}
			case ICON_REDO_PRESS:
			{
				result = imageMap.find("/MEDIP_Icon_Redo_MouseDown.png");
				break;
			}
			case ICON_REDO_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Redo_MouseOver.png");
				break;
			}
			case ICON_RESET:
			{
				result = imageMap.find("/MEDIP_Icon_Reset_MouseDown.png");
				break;
			}
			case ICON_NON_RESET:
			{
				result = imageMap.find("/MEDIP_Icon_Reset.png");
				break;
			}
			case ICON_RESET_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Reset_MouseOver.png");
				break;
			}
			case ICON_VIEW_FIT_COORD:
			{
				result = imageMap.find("/MEDIP_Icon_FitBoundingBox_MouseDown.png");
				break;
			}
			case ICON_VIEW_FIT_COORD_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_FitBoundingBox_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_FIT_COORD:
			{
				result = imageMap.find("/MEDIP_Icon_FitBoundingBox.png");
				break;
			}
			case ICON_DEEPCATCH_VIEW_L3:
			{
				result = imageMap.find("/MEDIP_Icon_L3_press.png");
				break;
			}
			case ICON_DEEPCATCH_VIEW_NON_L3:
			{
				result = imageMap.find("/MEDIP_Icon_L3_default.png");
				break;
			}
			case ICON_DEEPCATCH_VIEW_L3_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_L3_hover.png");
				break;
			}
			case ICON_DEEPCATCH_VIEW_AW:
			{
				result = imageMap.find("/MEDIP_Icon_AW_press.png");
				break;
			}
			case ICON_DEEPCATCH_VIEW_NON_AW:
			{
				result = imageMap.find("/MEDIP_Icon_AW_default.png");
				break;
			}
			case ICON_DEEPCATCH_VIEW_AW_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_AW_hover.png");
				break;
			}
			case ICON_APP:
			{
#if defined ALPHA_VERSION
				result = imageMap.find("/logo_alpha.ico");
#elif defined BETA_VERSION
				result = imageMap.find("/logo_beta.ico");
#else
				result = imageMap.find("/logo.ico");
#endif
				break;
			}
			case ICON_LIST_VISIBLE:
			{
				result = imageMap.find("/MEDIP_Icon_Eye_Icon.png");
				break;
			}
			case ICON_LIST_INVISIBLE:
			{
				result = imageMap.find("/MEDIP_Icon_Eye_Icon_Closed.png");
				break;
			}
			case ICON_LIST_LOCKER_CLOSE:
			{
				result = imageMap.find("/MEDIP_Icon_locker_close.png");
				break;
			}
			case ICON_LIST_LOCKER_OPEN:
			{
				result = imageMap.find("/MEDIP_Icon_locker_open.png");
				break;
			}
			case ICON_VIEW_NON_POLYROI:
			{
				result = imageMap.find("/MEDIP_Icon_Figure.png");
				break;
			}
			case ICON_VIEW_POLYROI:
			{
				result = imageMap.find("/MEDIP_Icon_Figure_MouseDown.png");
				break;
			}
			case ICON_VIEW_POLYROI_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Figure_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_FREEDRAWROI:
			{
				result = imageMap.find("/MEDIP_Icon_PathPencil.png");
				break;
			}
			case ICON_VIEW_FREEDRAWROI:
			{
				result = imageMap.find("/MEDIP_Icon_PathPencil_MouseDown.png");
				break;
			}
			case ICON_VIEW_FREEDRAWROI_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_PathPencil_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_PICKERROI:
			{
				result = imageMap.find("/MEDIP_Icon_Pencil.png");
				break;
			}
			case ICON_VIEW_PICKERROI:
			{
				result = imageMap.find("/MEDIP_Icon_Pencil_MouseDown.png");
				break;
			}
			case ICON_VIEW_PICKERROI_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Pencil_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_SKETCHDRAWSEGMENTATION:
			{
				result = imageMap.find("/MEDIP_Icon_Drowcut.png");
				break;
			}
			case ICON_VIEW_SKETCHDRAWSEGMENTATION_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Drowcut_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_SKETCHDRAWSEGMENTATION_OK:
			{
				result = imageMap.find("/MEDIP_Icon_Drowcut_MouseDown.png");
				break;
			}
			case ICON_VIEW_SKETCHDRAWSEGMENTATION_OK:
			{
				result = imageMap.find("/MEDIP_Icon_Drowcut_MouseDown.png");
				break;
			}
			case ICON_VIEW_SKETCHDRAWSEGMENTATION_OK_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Drowcut_MouseDown.png");
				break;
			}
			case ICON_VIEW_NON_ANGLEDROI:
			{
				result = imageMap.find("/Medip_Icon_AngularDraw.png");
				break;
			}
			case ICON_VIEW_ANGLEDROI:
			{
				result = imageMap.find("/MEDIP_Icon_AngularDraw_MouseDown.png");
				break;
			}
			case ICON_VIEW_ANGLEDROI_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_AngularDraw_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_COORD:
			{
				result = imageMap.find("/MEDIP_Icon_TOI.png");
				break;
			}
			case ICON_VIEW_COORD:
			{
				result = imageMap.find("/MEDIP_Icon_TOI_MouseDown.png");
				break;
			}
			case ICON_VIEW_COORD_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_TOI_MouseOver.png");
				break;
			}
			case ICON_VIEW_NON_BOUNDARY:
			{
				result = imageMap.find("/MEDIP_Icon_Boundary.png");
				break;
			}
			case ICON_VIEW_NON_BOUNARY_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Boundary_MouseOver.png");
				break;
			}
			case ICON_VIEW_BOUNARY:
			{
				result = imageMap.find("/MEDIP_Icon_Boundary_MouseDown.png");
				break;
			}
			case ICON_VIEW_BOUNARY_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Boundary_MouseOver.png");
				break;
			}
			case ICON_NON_ANNO_TEXT:
			{
				result = imageMap.find("/MEDIP_Icon_Text.png");
				break;
			}
			case ICON_ANNO_TEXT:
			{
				result = imageMap.find("/MEDIP_Icon_Text_MouseDown.png");
				break;
			}
			case ICON_ANNO_TEXT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Text_MouseOver.png");
				break;
			}
			case ICON_ROILIST_NON_NEW:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_New.png");
				break;
			}
			case ICON_ROILIST_NEW:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_New_MouseDown.png");
				break;
			}
			case ICON_ROILIST_NEW_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_New_MouseOver.png");
				break;
			}
			case ICON_ROILIST_NON_COPY:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_copy.png");
				break;
			}
			case ICON_ROILIST_COPY:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_Copy_MouseDown.png");
				break;
			}
			case ICON_ROILIST_COPY_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_Copy_MouseOver.png");
				break;
			}
			case ICON_ROILIST_NON_DEL:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_Delete.png");
				break;
			}
			case ICON_ROILIST_DEL:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_Delete_MouseDown.png");
				break;
			}
			case ICON_ROILIST_DEL_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_Delete_MouseOver.png");
				break;
			}
			case ICON_ROILIST_MERGE:
			{
				result = imageMap.find("/MEDIP_Icon_MergeLayers_MouseDown.png");
				break;
			}
			case ICON_ROILIST_NON_MERGE:
			{
				result = imageMap.find("/MEDIP_Icon_MergeLayers.png");
				break;
			}
			case ICON_ROILIST_MERGE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_MergLayers_MouseOver.png");
				break;
			}
			case ICON_ROILIST_FFS:
			{
				result = imageMap.find("/MEDIP_Icon_SubtractLayers_MouseDown.png");
				break;
			}
			case ICON_ROILIST_FFS_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_SubtractLayers_MouseOver.png");
				break;
			}
			case ICON_ROILIST_NON_FFS:
			{
				result = imageMap.find("/MEDIP_Icon_SubtractLayers.png");
				break;
			}
			case ICON_FILE_NON_OPEN:
			{
				result = imageMap.find("/MEDIP_Icon_Open.png");
				break;
			}
			case ICON_FILE_OPEN:
			{
				result = imageMap.find("/MEDIP_Icon_Open_MouseDown.png");
				break;
			}
			case ICON_FILE_OPEN_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Open_MouseOver.png");
				break;
			}
			case ICON_FILE_NON_OPEN_FILEMANAGER:
			{
				result = imageMap.find("/MEDIP_Icon_FileManager_Open.png");
				break;
			}
			case ICON_FILE_OPEN_FILEMANAGER:
			{
				result = imageMap.find("/MEDIP_Icon_FileManager_Open_MouseDown.png");
				break;
			}
			case ICON_FILE_OPEN_FILEMANAGER_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_FileManager_Open_MouseOver.png");
				break;
			}
			case ICON_NON_APP_INFO:
			{
				result = imageMap.find("/MEDIP_Icon_MedipInfo.png");
				break;
			}
			case ICON_APP_INFO:
			{
				result = imageMap.find("/MEDIP_Icon_MedipInfo_MouseDown.png");
				break;
			}
			case ICON_APP_INFO_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_MedipInfo_MouseOver.png");
				break;
			}
			case ICON_NON_UPDATE_MASK:
			{
				result = imageMap.find("/MEDIP_Icon_UpdateMask.png");
				break;
			}
			case ICON_UPDATE_MASK:
			{
				result = imageMap.find("/MEDIP_Icon_UpdateMask_MouseDown.png");
				break;
			}
			case ICON_UPDATE_MASK_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_UpdateMask_MouseOver.png");
				break;
			}
			case ICON_NON_HISTOGRAM:
			{
				result = imageMap.find("/MEDIP_Icon_Histogram.png");
				break;
			}
			case ICON_NON_SVIEW:
			{
				result = imageMap.find("/MEDIP_Icon_CrossSection.png");
				break;
			}
			case ICON_HISTOGRAM:
			{
				result = imageMap.find("/MEDIP_Icon_Histogram_MouseDown.png");
				break;
			}
			case ICON_SVIEW:
			{
				result = imageMap.find("/MEDIP_Icon_CrossSection_MouseDown.png");
				break;
			}
			case ICON_HISTOGRAM_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Histogram_MouseOver.png");
				break;
			}
			case ICON_SVIEW_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_CrossSection_MouseOver.png");
				break;
			}
			case ICON_NON_ANNO_PATH:
			{
				result = imageMap.find("/MEDIP_Icon_CammeraAnimation.png");
				break;
			}
			case ICON_ANNO_PATH:
			{
				result = imageMap.find("/MEDIP_Icon_CammeraAnimation_MouseDown.png");
				break;
			}
			case ICON_ANNO_PATH_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_CammeraAnimation_MouseOver.png");
				break;
			}
			case ICON_NON_ANNO_PATH_PLAY:
			{
				result = imageMap.find("/MEDIP_Icon_CammeraAnimationPlay.png");
				break;
			}
			case ICON_ANNO_PATH_PLAY_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_CammeraAnimationPlay_MouseOver.png");
				break;
			}
			case ICON_NON_STOP:
			{
				result = imageMap.find("/MEDIP_Icon_CammeraAnimationStop.png");
				break;
			}
			case ICON_ANNO_PATH_PLAY:
			case ICON_STOP:
			{
				result = imageMap.find("/MEDIP_Icon_CammeraAnimationStop_MouseDown.png");
				break;
			}
			case ICON_STOP_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_CammeraAnimationStop_MouseOver.png");
				break;
			}
			case ICON_NON_MIX:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_the_volume_with_the_layers.png");
				break;
			}
			case ICON_MIX:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_the_volume_with_the_layers_MouseDown.png");
				break;
			}
			case ICON_MIX_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Mask_the_volume_with_the_layers_MouseOver.png");
				break;
			}
			case ICON_NON_PLANE_CUT:
			{
				result = imageMap.find("/MEDIP_Plane3DCuttingOff.png");
				break;
			}
			case ICON_PLANE_CUT:
			{
				result = imageMap.find("/MEDIP_Plane3DCuttingOff_MouseDown.png");
				break;
			}
			case ICON_PLANE_CUT_HOVER:
			{
				result = imageMap.find("/MEDIP_Plane3DCuttingOff_MouseOver.png");
				break;
			}
			case ICON_NON_BRUSH_SCULPT:
			{
				result = imageMap.find("/MEDIP_BrushSculpt.png");
				break;
			}
			case ICON_BRUSH_SCULPT:
			{
				result = imageMap.find("/MEDIP_BrushSculpt_MouseDown.png");
				break;
			}
			case ICON_BRUSH_SCULPT_HOVER:
			{
				result = imageMap.find("/MEDIP_BrushSculpt_MouseOver.png");
				break;
			}
			case ICON_NON_BRUSH_OP_SURFACE:
			{
				result = imageMap.find("/MEDIP_BrushOp_Surface.png");
				break;
			}
			case ICON_BRUSH_OP_SURFACE:
			{
				result = imageMap.find("/MEDIP_BrushOp_Surface_MouseDown.png");
				break;
			}
			case ICON_BRUSH_OP_SURFACE_HOVER:
			{
				result = imageMap.find("/MEDIP_BrushOp_Surface_MouseOver.png");
				break;
			}
			case ICON_NON_BRUSH_OP_VOLUME:
			{
				result = imageMap.find("/MEDIP_BrushOp_Volume.png");
				break;
			}
			case ICON_BRUSH_OP_VOLUME:
			{
				result = imageMap.find("/MEDIP_BrushOp_Volume_MouseDown.png");
				break;
			}
			case ICON_BRUSH_OP_VOLUME_HOVER:
			{
				result = imageMap.find("/MEDIP_BrushOp_Volume_MouseOver.png");
				break;
			}
			case ICON_NON_SCULPT_OP_SMOOTH:
			{
				result = imageMap.find("/MEDIP_SculptOp_Smooth.png");
				break;
			}
			case ICON_SCULPT_OP_SMOOTH:
			{
				result = imageMap.find("/MEDIP_SculptOp_Smooth_MouseDown.png");
				break;
			}
			case ICON_SCULPT_OP_SMOOTH_HOVER:
			{
				result = imageMap.find("/MEDIP_SculptOp_Smooth_MouseOver.png");
				break;
			}
			case ICON_NON_SCULPT_OP_INFLATE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Inflate.png");
				break;
			}
			case ICON_SCULPT_OP_INFLATE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Inflate_MouseDown.png");
				break;
			}
			case ICON_SCULPT_OP_INFLATE_HOVER:
			{
				result = imageMap.find("/MEDIP_SculptOp_Inflate_MouseOver.png");
				break;
			}
			case ICON_NON_SCULPT_OP_DEFLATE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Deflate.png");
				break;
			}
			case ICON_SCULPT_OP_DEFLATE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Deflate_MouseDown.png");
				break;
			}
			case ICON_SCULPT_OP_DEFLATE_HOVER:
			{
				result = imageMap.find("/MEDIP_SculptOp_Deflate_MouseOver.png");
				break;
			}
			case ICON_NON_SCULPT_OP_REDUCE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Reduce.png");
				break;
			}
			case ICON_SCULPT_OP_REDUCE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Reduce_MouseDown.png");
				break;
			}
			case ICON_SCULPT_OP_REDUCE_HOVER:
			{
				result = imageMap.find("/MEDIP_SculptOp_Reduce_MouseOver.png");
				break;
			}
			case ICON_NON_SCULPT_OP_REFINE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Refine.png");
				break;
			}
			case ICON_SCULPT_OP_REFINE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Refine_MouseDown.png");
				break;
			}
			case ICON_SCULPT_OP_REFINE_HOVER:
			{
				result = imageMap.find("/MEDIP_SculptOp_Refine_MouseOver.png");
				break;
			}
			case ICON_NON_SCULPT_OP_MOVE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Move.png");
				break;
			}
			case ICON_SCULPT_OP_MOVE:
			{
				result = imageMap.find("/MEDIP_SculptOp_Move_MouseDown.png");
				break;
			}
			case ICON_SCULPT_OP_MOVE_HOVER:
			{
				result = imageMap.find("/MEDIP_SculptOp_Move_MouseOver.png");
				break;
			}
			case ICON_NON_SCULPT_OP_DRAG:
			{
				result = imageMap.find("/MEDIP_SculptOp_Drag.png");
				break;
			}
			case ICON_SCULPT_OP_DRAG:
			{
				result = imageMap.find("/MEDIP_SculptOp_Drag_MouseDown.png");
				break;
			}
			case ICON_SCULPT_OP_DRAG_HOVER:
			{
				result = imageMap.find("/MEDIP_SculptOp_Drag_MouseOver.png");
				break;
			}
			case ICON_NON_SCULPT_OP_HOLEFILL:
			{
				result = imageMap.find("/MEDIP_SculptOp_HoleFill.png");
				break;
			}
			case ICON_SCULPT_OP_HOLEFILL:
			{
				result = imageMap.find("/MEDIP_SculptOp_HoleFill_MouseDown.png");
				break;
			}
			case ICON_SCULPT_OP_HOLEFILL_HOVER:
			{
				result = imageMap.find("/MEDIP_SculptOp_HoleFill_MouseOver.png");
				break;
			}
			case ICON_NON_BRUSH_SELECT:
			{
				result = imageMap.find("/MEDIP_Select.png");
				break;
			}
			case ICON_BRUSH_SELECT:
			{
				result = imageMap.find("/MEDIP_Select_MouseDown.png");
				break;
			}
			case ICON_BRUSH_SELECT_HOVER:
			{
				result = imageMap.find("/MEDIP_Select_MouseOver.png");
				break;
			}
			case ICON_NON_MESH_OBJECTS:
			{
				result = imageMap.find("/MEDIP_MeshObjects.png");
				break;
			}
			case ICON_MESH_OBJECTS:
			{
				result = imageMap.find("/MEDIP_MeshObjects_MouseDown.png");
				break;
			}
			case ICON_MESH_OBJECTS_HOVER:
			{
				result = imageMap.find("/MEDIP_MeshObjects_MouseOver.png");
				break;
			}
			case ICON_NON_ALPHA_IMG:
			{
				result = imageMap.find("/MEDIP_Icon_ShowImage_MouseDown.png");
				break;
			}
			case ICON_ALPHA_IMG:
			{
				result = imageMap.find("/MEDIP_Icon_ShowImage.png");
				break;
			}
			case ICON_ALPHA_IMG_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_ShowImage_MouseOver.png");
				break;
			}
			case ICON_INACT:
			{
				result = imageMap.find("/MEDIP_Icon_ShowImage.png");
				break;
			}
			case ICON_NON_INACT:
			{
				result = imageMap.find("/MEDIP_Icon_ShowImage_MouseDown.png");
				break;
			}
			case ICON_INACT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_ShowImage_MouseOver.png");
				break;
			}
			case ICON_LINK:
			{
				result = imageMap.find("/MEDIP_Icon_ConnectToMedicalip_MouseDown.png");
				break;
			}
			case ICON_NON_LINK:
			{
				result = imageMap.find("/MEDIP_Icon_ConnectToMedicalip.png");
				break;
			}
			case ICON_LINK_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_ConnectToMedicalip_MouseOver.png");
				break;
			}
			case ICON_CLEAR:
			{
				result = imageMap.find("/MEDIP_Icon_ROI_Clear.png");
				break;
			}
			case ICON_ANIMATION_SINGLE:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B.png");
				break;
			}
			case ICON_ANIMATION_PLAY_AB:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B-A-B.png");
				break;
			}
			case ICON_ANIMATION_PLAY_BA:
			{
				result = imageMap.find("/MEDIP_Icon_PathAnimationA-B-B-A.png");
				break;
			}
			case ICON_REPORT_NEW_FORMAT:
			{
				result = imageMap.find("/MEDIP_Icon_CreateNewFormat_MouseDown.png");
				break;
			}
			case ICON_REPORT_NEW_FORMAT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_CreateNewFormat_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_NEW_FORMAT:
			{
				result = imageMap.find("/MEDIP_Icon_CreateNewFormat.png");
				break;
			}
			case ICON_REPORT_SAVE_FORMAT:
			{
				result = imageMap.find("/MEDIP_Icon_SaveFormat_MouseDown.png");
				break;
			}
			case ICON_REPORT_SAVE_FORMAT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_SaveFormat_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_SAVE_FORMAT:
			{
				result = imageMap.find("/MEDIP_Icon_SaveFormat.png");
				break;
			}
			case ICON_REPORT_SAVE_AS_FORMAT:
			{
				result = imageMap.find("/MEDIP_Icon_SaveAsFormat_MouseDown.png");
				break;
			}
			case ICON_REPORT_SAVE_AS_FORMAT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_SaveAsFormat_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_SAVE_AS_FORMAT:
			{
				result = imageMap.find("/MEDIP_Icon_SaveAsFormat.png");
				break;
			}
			case ICON_REPORT_DELETE_FORMAT:
			{
				result = imageMap.find("/MEDIP_Icon_DeleteFormat_MouseDown.png");
				break;
			}
			case ICON_REPORT_DELETE_FORMAT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_DeleteFormat_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_DELETE_FORMAT:
			{
				result = imageMap.find("/MEDIP_Icon_Delete.png");
				break;
			}
			case ICON_REPORT_TEXT_COLOR:
			{
				result = imageMap.find("/MEDIP_Icon_Report_Text_Color_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_COLOR_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Report_Text_Color_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_COLOR:
			{
				result = imageMap.find("/MEDIP_Icon_Report_Text_Color.png");
				break;
			}
			case ICON_REPORT_TEXT_BOLD:
			{
				result = imageMap.find("/MEDIP_Icon_Bold_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_BOLD_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Bold_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_BOLD:
			{
				result = imageMap.find("/MEDIP_Icon_Bold.png");
				break;
			}
			case ICON_REPORT_TEXT_ITALIC:
			{
				result = imageMap.find("/MEDIP_Icon_Italic_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_ITALIC_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Italic_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_ITALIC:
			{
				result = imageMap.find("/MEDIP_Icon_Italic.png");
				break;
			}
			case ICON_REPORT_TEXT_UNDER_LINE:
			{
				result = imageMap.find("/MEDIP_Icon_Underline_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_UNDER_LINE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Underline_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_UNDER_LINE:
			{
				result = imageMap.find("/MEDIP_Icon_Underline.png");
				break;
			}
			case ICON_REPORT_TEXT_LEFT_ALIGN:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Left_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_LEFT_ALIGN_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Left_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_LEFT_ALIGN:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Left.png");
				break;
			}
			case ICON_REPORT_TEXT_CENTER_ALIGN:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Center_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_CENTER_ALIGN_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Center_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_CENTER_ALIGN:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Center.png");
				break;
			}
			case ICON_REPORT_TEXT_RIGHT_ALIGN:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Right_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_RIGHT_ALIGN_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Right.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_RIGHT_ALIGN:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Right.png");
				break;
			}
			case ICON_REPORT_TEXT_JUSTIFY_ALIGN:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Justify_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_JUSTIFY_ALIGN_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Justify_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_JUSTIFY_ALIGN:
			{
				result = imageMap.find("/MEDIP_Icon_TextAlign-Justify.png");
				break;
			}
			case ICON_REPORT_TEXT_HEADER_LIST:
			{
				result = imageMap.find("/MEDIP_Icon_HeadersList_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_HEADER_LIST_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_HeadersList_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_HEADER_LIST:
			{
				result = imageMap.find("/MEDIP_Icon_HeadersList.png");
				break;
			}
			case ICON_REPORT_TEXT_HEADER_SQUARE:
			{
				result = imageMap.find("/MEDIP_Icon_Square.png");
				break;
			}
			case ICON_REPORT_TEXT_HEADER_DISC:
			{
				result = imageMap.find("/MEDIP_Icon_Disc.png");
				break;
			}
			case ICON_REPORT_TEXT_HEADER_CIRCLE:
			{
				result = imageMap.find("/MEDIP_Icon_Circle.png");
				break;
			}
			case ICON_REPORT_TEXT_SEQUENCE_LIST:
			{
				result = imageMap.find("/MEDIP_Icon_SequenceList_MouseDown.png");
				break;
			}
			case ICON_REPORT_TEXT_SEQUENCE_LIST_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_SequenceList_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_TEXT_SEQUENCE_LIST:
			{
				result = imageMap.find("/MEDIP_Icon_SequenceList.png");
				break;
			}
			case ICON_REPORT_TEXT_SEQUENCE_DECIMAL:
			{
				result = imageMap.find("/MEDIP_Icon_Decimal.png");
				break;
			}
			case ICON_REPORT_TEXT_SEQUENCE_LOWER_ALPHA:
			{
				result = imageMap.find("/MEDIP_Icon_LowerAlpha.png");
				break;
			}
			case ICON_REPORT_TEXT_SEQUENCE_UPPER_ALPHA:
			{
				result = imageMap.find("/MEDIP_Icon_UpperAlpha.png");
				break;
			}
			case ICON_REPORT_TEXT_SEQUENCE_LOWER_ROMAN:
			{
				result = imageMap.find("/MEDIP_Icon_LowerRoman.png");
				break;
			}
			case ICON_REPORT_TEXT_SEQUENCE_UPPER_ROMAN:
			{
				result = imageMap.find("/MEDIP_Icon_UpperRoman.png");
				break;
			}
			case ICON_REPORT_CREATE_TABLE:
			{
				result = imageMap.find("/MEDIP_Icon_CreateTable_MouseDown.png");
				break;
			}
			case ICON_REPORT_CREATE_TABLE_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_CreateTable_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_CREATE_TABLE:
			{
				result = imageMap.find("/MEDIP_Icon_CreateTable.png");
				break;
			}
			case ICON_REPORT_MERGE_CELL:
			{
				result = imageMap.find("/MEDIP_Icon_MergeCell_MouseDown.png");
				break;
			}
			case ICON_REPORT_MERGE_CELL_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_MergeCell_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_MERGE_CELL:
			{
				result = imageMap.find("/MEDIP_Icon_MergeCell.png");
				break;
			}
			case ICON_REPORT_SPLIT_CELL:
			{
				result = imageMap.find("/MEDIP_Icon_SplitCell_MouseDown.png");
				break;
			}
			case ICON_REPORT_SPLIT_CELL_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_SplitCell_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_SPLIT_CELL:
			{
				result = imageMap.find("/MEDIP_Icon_SplitCell.png");
				break;
			}
			case ICON_REPORT_INSERT_ROW:
			{
				result = imageMap.find("/MEDIP_Icon_InsertRow_MouseDown.png");
				break;
			}
			case ICON_REPORT_INSERT_ROW_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_InsertRow_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_INSERT_ROW:
			{
				result = imageMap.find("/MEDIP_Icon_InsertRow.png");
				break;
			}
			case ICON_REPORT_INSERT_COLUMN:
			{
				result = imageMap.find("/MEDIP_Icon_InsertColumn_MouseDown.png");
				break;
			}
			case ICON_REPORT_INSERT_COLUMN_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_InsertColumn_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_INSERT_COLUMN:
			{
				result = imageMap.find("/MEDIP_Icon_InsertColumn.png");
				break;
			}
			case ICON_REPORT_CELL_PROPERTY:
			{
				result = imageMap.find("/MEDIP_Icon_CellProperty_MouseDown.png");
				break;
			}
			case ICON_REPORT_CELL_PROPERTY_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_CellProperty_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_CELL_PROPERTY:
			{
				result = imageMap.find("/MEDIP_Icon_CellProperty.png");
				break;
			}
			case ICON_REPORT_IMPORT:
			{
				result = imageMap.find("/MEDIP_Icon_Import_MouseDown.png");
				break;
			}
			case ICON_REPORT_IMPORT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Import_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_IMPORT:
			{
				result = imageMap.find("/MEDIP_Icon_Import.png");
				break;
			}
			case ICON_REPORT_EXPORT:
			{
				result = imageMap.find("/MEDIP_Icon_Export_MouseDown.png");
				break;
			}
			case ICON_REPORT_EXPORT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Export_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_EXPORT:
			{
				result = imageMap.find("/MEDIP_Icon_Export.png");
				break;
			}
			case ICON_REPORT_PRINT:
			{
				result = imageMap.find("/MEDIP_Icon_Print_MouseDown.png");
				break;
			}
			case ICON_REPORT_PRINT_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_Print_MouseOver.png");
				break;
			}
			case ICON_NON_REPORT_PRINT:
			{
				result = imageMap.find("/MEDIP_Icon_Print.png");
				break;
			}
			case ICON_IMAGE_MANAGEMENT_TAB:
			{
				result = imageMap.find("/MEDIP_Icon_ShowHideImageList_MouseDown.png");
				break;
			}
			case ICON_IMAGE_MANAGEMENT_TAB_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_ShowHideImageList_MouseOver.png");
				break;
			}
			case ICON_NON_IMAGE_MANAGEMENT_TAB:
			{
				result = imageMap.find("/MEDIP_Icon_ShowHideImageList.png");
				break;
			}
			case ICON_NON_RADIOMICS_HEIGHTMAP:
			{
				result = imageMap.find("/MEDIP_Icon_HeightMap.png");
				break;
			}
			case ICON_RADIOMICS_HEIGHTMAP:
			{
				result = imageMap.find("/MEDIP_Icon_HeightMap-MouseDown.png");
				break;
			}
			case ICON_RADIOMICS_HEIGHTMAP_HOVER:
			{
				result = imageMap.find("/MEDIP_Icon_HeightMap-MouseOver.png");
				break;
			}
			case ICON_STATUS_BAR_GRAB_RESIZE:
			{
				result = imageMap.find("/MEDIP_Icon_StatusBar_GrabResize.png");
				break;
			}
			default:
			{
				result = imageMap.find("/MEDIP_Icon_Open.png");
				break;
			}

		}

		if (result != imageMap.end())
			pix.loadFromData(*result.value());

		if (width && height)
			pix = pix.scaled(width, height);
		return QIcon(pix);
	}
}


bool ResourceManager::init()
{
	bool  resourceData = false;

	// cache 디렉토리 정보 먼저 읽어오기.
	QString cachePath;
	if (WIN_MANAGER->getConfigValue(ELEMENT_FILE, STRING_MANAGER->config_cachePath, cachePath))
	{
		if (STRING_MANAGER->cacheFilePath != cachePath)
		{
			QDir dir(cachePath);

			if (!dir.exists())
				dir.mkdir(cachePath);

			STRING_MANAGER->cacheFilePath = cachePath;
		}
	}
	STRING_MANAGER->cacheFileOriginVolume = STRING_MANAGER->cacheFilePath + "/core.data";

	QDir dir(STRING_MANAGER->cacheFilePath);
	if (dir.exists())
		dir.removeRecursively();

	if (1)
	{
		QFile file(STRING_MANAGER->resourceFile);
#ifdef _DEBUG	
#ifdef OVERWRITE_MEDITIM_DAT_FILE
		{
			FolderCompressor comp;
			comp.compressFolder(STRING_MANAGER->resourceDir, STRING_MANAGER->resourceFile);
		}
#else
		if (file.exists() == false)
		{
			FolderCompressor comp;
			comp.compressFolder(STRING_MANAGER->resourceDir, STRING_MANAGER->resourceFile);
		}
#endif //OVERWRITE_MEDITIM_DAT_FILE	
#endif //_DEBUG

		if (file.exists() == false)
		{
			return false;
		}

		FolderCompressor comp;
		if (comp.loadResourceFromCompressFile(STRING_MANAGER->resourceFile) == false)
		{
			return false;
		}

		resourceData = true;
	}

	/*
	QFile file(STRING_MANAGER->resourceFile);
	if (file.exists() == true)
	{

		file.open(QIODevice::ReadOnly);
		SET_BINARY_MODE(stdin);
		SET_BINARY_MODE(stdout);

		int ret = 0;
		if (0) // zip
		{
			ret = zip(stdin, stdout, Z_DEFAULT_COMPRESSION);
		}
		else // unzip
		{
			ret = unzip(stdin, stdout);
		}

		if (ret != Z_OK)
		{
			zerr(ret);
		}
	}
	*/

	if (!dir.mkdir(STRING_MANAGER->cacheFilePath))
		return false;

	QDir dumpdir(STRING_MANAGER->dumpFilePath);
	if (!dumpdir.exists())
	{
		dir.mkdir(STRING_MANAGER->dumpFilePath);
	}

	{
		QByteArray * _data = NULL;
		auto r = imageMap.find("/MEDIP_Icon_TopView_Icon.png");
		if (r != imageMap.end())
			_data = r.value();

		imageUnloadAxial = new QImage();
		if (resourceData)
			imageUnloadAxial->loadFromData(*_data);
		else
			imageUnloadAxial->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Render_MouseOver.png"));



		r = imageMap.find("/MEDIP_Icon_FrontView_Icon.png");
		if (r != imageMap.end())
			_data = r.value();

		imageUnloadCoronal = new QImage();
		if (resourceData)
			imageUnloadCoronal->loadFromData(*_data);
		else
			imageUnloadCoronal->load(QString(STRING_MANAGER->resourceDir) + QString("/MEDIP_Icon_Render_MouseOver.png"));


		r = imageMap.find("/MEDIP_Icon_SideView_Icon.png");
		if (r != imageMap.end())
			_data = r.value();

		imageUnloadSagittal = new QImage();
		if (resourceData)
			imageUnloadSagittal->loadFromData(*_data);
		else
			imageUnloadSagittal->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Render_MouseOver.png"));

		imageUnloadSubView = new QImage();
		if (resourceData)
			imageUnloadSubView->loadFromData(*_data);
		else
			imageUnloadSubView->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Render_MouseOver.png"));

		r = imageMap.find("/MEDIP_Icon_Render.png");
		if (r != imageMap.end())
			_data = r.value();

		imageUnloadVolume = new QImage();
		if (resourceData)
			imageUnloadVolume->loadFromData(*_data);
		else
			imageUnloadVolume->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Render_MouseOver.png"));

		r = imageMap.find("/MEDIP_Icon_WaterMark.png");
		if (r != imageMap.end())
			_data = r.value();

		imageWaterMark = new QImage();
		if (resourceData)
			imageWaterMark->loadFromData(*_data);
		else
			imageWaterMark->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_WaterMark.png"));


		r = imageMap.find("/MEDIP_Tex_Culling.png");
		if (r != imageMap.end())
			_data = r.value();

		image3DCullingPlane = new QImage();
		if (resourceData)
			image3DCullingPlane->loadFromData(*_data);
		else
			image3DCullingPlane->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Tex_Culling.png"));


		r = imageMap.find("/MEDIP_Tex_NotoSansRegular.png");
		if (r != imageMap.end())
			_data = r.value();

		image3DFontNotoSansRegular = new QImage();
		if (resourceData)
			image3DFontNotoSansRegular->loadFromData(*_data);
		else
			image3DFontNotoSansRegular->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Tex_NotoSansRegular.png"));


		r = imageMap.find("/MEDIP_Icon_Right.png");
		if (r != imageMap.end())
			_data = r.value();

		imageRight = new QImage();
		if (resourceData)
			imageRight->loadFromData(*_data);
		else
			imageRight->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Right.png"));

		r = imageMap.find("/MEDIP_Icon_Left.png");
		if (r != imageMap.end())
			_data = r.value();

		imageLeft = new QImage();
		if (resourceData)
			imageLeft->loadFromData(*_data);
		else
			imageLeft->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Left.png"));

		r = imageMap.find("/MEDIP_Icon_Anterior1.png");
		if (r != imageMap.end())
			_data = r.value();

		imageAnterior = new QImage();
		if (resourceData)
			imageAnterior->loadFromData(*_data);
		else
			imageAnterior->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Anterior1.png"));

		r = imageMap.find("/MEDIP_Icon_Posterior1.png");
		if (r != imageMap.end())
			_data = r.value();

		imagePosterior = new QImage();
		if (resourceData)
			imagePosterior->loadFromData(*_data);
		else
			imagePosterior->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Posterior1.png"));

		r = imageMap.find("/MEDIP_Icon_Anterior_2.png");
		if (r != imageMap.end())
			_data = r.value();

		imageAnteriorHori = new QImage();
		if (resourceData)
			imageAnteriorHori->loadFromData(*_data);
		else
			imageAnteriorHori->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Anterior_2.png"));

		r = imageMap.find("/MEDIP_Icon_Posterior_2.png");
		if (r != imageMap.end())
			_data = r.value();

		imagePosteriorHori = new QImage();
		if (resourceData)
			imagePosteriorHori->loadFromData(*_data);
		else
			imagePosteriorHori->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Posterior_2.png"));

		r = imageMap.find("/MEDIP_Icon_Superior.png");
		if (r != imageMap.end())
			_data = r.value();

		imageSuperior = new QImage();
		if (resourceData)
			imageSuperior->loadFromData(*_data);
		else
			imageSuperior->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Superior.png"));

		r = imageMap.find("/MEDIP_Icon_Inferior.png");
		if (r != imageMap.end())
			_data = r.value();

		imageInferior = new QImage();
		if (resourceData)
			imageInferior->loadFromData(*_data);
		else
			imageInferior->load(QString(STRING_MANAGER->resourceDir)+ QString("/MEDIP_Icon_Inferior.png"));

		//////////////////////////////////////////////////////////////////////////////
	}

	unloadBackGroundColor = QColor(48, 48, 48, 255);

	fontNormal.setFamily("Arial Regular");
	fontNormal.setPointSize(10);

	return true;
}

QImage * ResourceManager::getUnloadImage(WINDOW_TYPE type)
{
	switch (type)
	{
	case WT_CORONAL:
		return imageUnloadCoronal;
	case WT_SAGITTAL:
		return imageUnloadSagittal;
	case WT_AXIAL:
		return imageUnloadAxial;
	case WT_VOLUME:
		return imageUnloadVolume;
	case WT_SUB:
		return imageUnloadSubView;
	default:
		break;
	}

	return NULL;
}


/*@function getDirectionImage
 *@brief window direction image return
 *@param type window type (exclude volume window)
 *@param position top, bottom, right, left for window (top : 0, left : 3)
 *@return position image for window type
*/
QImage * ResourceManager::getDirectionImage(WINDOW_TYPE type, int position)
{
	switch (type)
	{
	case WT_CORONAL:
		if (POS_TOP == position)
			return imageSuperior;
		else if (POS_BOTTOM == position)
			return imageInferior;
		else if (POS_LEFT == position)
			return imageRight;
		else
			return imageLeft;
	case WT_SAGITTAL:
		if (POS_TOP == position)
			return imageSuperior;
		else if (POS_BOTTOM == position)
			return imageInferior;
		else if (POS_LEFT == position)
			return imageAnteriorHori;
		else
			return imagePosteriorHori;
	case WT_AXIAL:
		if (POS_TOP == position)
			return imageAnterior;
		else if (POS_BOTTOM == position)
			return imagePosterior;
		else if (POS_LEFT == position)
			return imageRight;
		else
			return imageLeft;
	}

	return NULL;
}

QImage * ResourceManager::getWaterMark()
{
	return imageWaterMark;
}

QIcon ResourceManager::GetIcon_ListVisible(bool visible)
{
	return visible ? RESOURCE_MANAGER->getIcon(ICON_LIST_VISIBLE) : RESOURCE_MANAGER->getIcon(ICON_LIST_INVISIBLE);
}

QIcon ResourceManager::GetIcon_ListOmniverseLock(eOmniverseStatus status)
{
	return status == eOmniverseStatus::Lock ? RESOURCE_MANAGER->getIcon(ICON_LIST_LOCKER_CLOSE) : RESOURCE_MANAGER->getIcon(ICON_LIST_LOCKER_OPEN);
}

