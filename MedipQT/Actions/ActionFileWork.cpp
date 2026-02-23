#include "stdafx.h"

#include "vtkAutoInit.h" 
VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);


#include "ActionFileWork.h"
//#include <QtWidgets>
#include "Windows/WindowBase.h"
#include "Windows/windowManager.h"
#include "system/stringManager.h"
#include "qfile.h"


#include "Renderer/Mesh.h"

#include "Windows/Main/MainSegmentWidget.h"
#include "Windows/Main/MainAnalWidget.h"
#include "Windows/Main/MainTAWidget.h"
#include "Windows/VolumeView.h"
#include "Windows/Tabwindow.h"
#include "Windows/AnalVolumeView.h"
#include "Windows/ReportWidget.h"

#include "Renderer/model.h"
#include "ActionManager.h"
#include "UI/CustomHistogram.h"

#include "defineMipEncoder.h"
#include "medipmipencoder.h"

#include "MeshControl.h"
#include "MeshEdit/CMeshWorkManager.h"
#include "3MF/C3MFLoader.h"

#include "System\LicenseManager.h"

#include "MeshEdit\CMeshModelViewManager.h"
#include "MeshEdit\CManipulator.h"

#include <vtkUnsignedCharArray.h>
#include <NRRD/NRRD/nrrd.h>
#include <NRRD/NRRD/nrrd_image.h>
#include <NRRD/NRRD/nrrd_image_view.h>
#include <ppl.h>

#define  USE_PPL

using namespace std;







