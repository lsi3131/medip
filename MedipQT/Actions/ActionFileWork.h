#pragma once
#include "define.h"
#include <QtWidgets>
#include <QUndoCommand>
#include <qpoint.h>
#include <Qthread>

#include "graphics/BoundingBox.h"
#include "graphics/volumedata.h"

#include "MeshWork/Brush/WorkBrushThread.h"
#include "FileWork/Mesh/WorkLoad3MF.h"
#include "FileWork/Mesh/WorkLoadOBJ.h"
#include "FileWork/Mesh/WorkLoadSTL.h"
#include "FileWork/Mesh/WorkMeshTo3MF.h"
#include "FileWork/Mesh/WorkMeshToOBJ.h"
#include "FileWork/Mesh/WorkMeshToSTL.h"
#include "FileWork/Mesh/WorkMeshToVTK.h"
#include "FileWork/Mesh/WorkSurfaceSaveOBJ.h"
#include "FileWork/Mesh/WorkSurfaceSaveSTL.h"
#include "FileWork/Mesh/WorkSurfaceSaveVTK.h"
#include "FileWork/Mesh/WorkVTKOpen.h"
#include "FileWork/WorkDicomOpen.h"
#include "FileWork/WorkMipOpen.h"
#include "FileWork/WorkNIIOpen.h"
#include "FileWork/WorkRawOpen.h"
#include "FileWork/WorkROIOpen.h"
#include "FileWork/WorkSaveCoordinateNII.h"
#include "FileWork/WorkSaveHUNII.h"
#include "FileWork/WorkSaveHURAW.h"
#include "FileWork/WorkSaveJSON.h"
#include "FileWork/WorkSaveMIP.h"
#include "FileWork/WorkSaveNII.h"
#include "FileWork/WorkSaveNRRD.h"
#include "FileWork/WorkSaveRAW.h"
#include "FileWork/WorkSaveROINII.h"
#include "FileWork/WorkSaveSplits.h"
#include "FileWork/WorkSaveTXT.h"
#include "FileWork/WorkTXTOpen.h"

#define MIP_ENCODER_TEST

namespace mip
{
	class MeshTopology;
};

class vtkObject;
class WindowBase;
class MyThreadQT;
class VOLUME_DATA;









