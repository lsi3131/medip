#include "stdafx.h"
#include "C3MFLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "Windows/windowManager.h"
#define USE_OPENGL
#include "Renderer/texture.h"
#include "Renderer/model.h"
#include "Renderer/ShaderCommon.h"
#include "System/StringManager.h"

#include <QFile>
#include <ppl.h>
using namespace concurrency;
//#define USE_PPL

namespace mip
{
	namespace lib3mf
	{
		C3MFLoader::C3MFLoader()
		{
		}
		C3MFLoader::~C3MFLoader()
		{
		}

		void C3MFLoader::Save3mfFile_sample(std::string sFileName, mip::MeshTopology * in_pMT) {
			PWrapper wrapper = CWrapper::loadLibrary();

			std::cout << "------------------------------------------------------------------" << std::endl;
			std::cout << "3MF Save" << std::endl;
			printVersion(wrapper);
			std::cout << "------------------------------------------------------------------" << std::endl;

			PModel model = wrapper->CreateModel();
			
			//stbi_write_png(filename, data.w, data.h, data.bpp, data.data, 0);

			PMeshObject meshObject = model->AddMeshObject();
			meshObject->SetName("Textured Box");

			// Create mesh structure of a cube
			std::vector<sLib3MFPosition> vertices;
			std::vector<sLib3MFTriangle> triangles;

			Create3mfVertexFromTopology(in_pMT, vertices);
			Create3mfTriFromTopology(in_pMT, triangles);

			meshObject->SetGeometry(vertices, triangles);

			int size = meshObject->GetTriangleCount();
			
			for (int ii = 0; ii < size; ii++)
			{
				sTriangleProperties prop;
				meshObject->GetTriangleProperties(ii, prop);
				printf_s("\n tri - %d / ResourceID - %d / m_PropertyIDs - %d, %d, %d", ii, prop.m_ResourceID, prop.m_PropertyIDs[0], prop.m_PropertyIDs[1], prop.m_PropertyIDs[2]);
				//printf_s("\n ResourceID - %d / [U1_%.4f , V1_%.4f] / [U2_%.4f , V2_%.4f] / [U3_%.4f , V3_%.4f]", prop.m_ResourceID, prop.m_PropertyIDs[0], v1, u2, v2, u3, v3);
			}


			std::string sTextureFolder = "D:/3D/Texture/";

			// add textures to 3mf package
			std::cout << "sTextureFolder=\"" << sTextureFolder << "\"\n";
			PTexture2DGroup textureGroup1 = fnLoadModelTexture(model, "/3D/Textures/tex1.png", sTextureFolder + "text1.png", eTextureType::PNG, eTextureTileStyle::Wrap, eTextureTileStyle::Wrap);
			PTexture2DGroup textureGroup2 = fnLoadModelTexture(model, "/3D/Textures/tex2.png", sTextureFolder + "text2.png", eTextureType::PNG, eTextureTileStyle::Mirror, eTextureTileStyle::Wrap);
			PTexture2DGroup textureGroup3 = fnLoadModelTexture(model, "/3D/Textures/tex3.png", sTextureFolder + "text3.png", eTextureType::PNG, eTextureTileStyle::Wrap, eTextureTileStyle::Mirror);
			PTexture2DGroup textureGroup4 = fnLoadModelTexture(model, "/3D/Textures/tex4.png", sTextureFolder + "text4.png", eTextureType::PNG, eTextureTileStyle::Clamp, eTextureTileStyle::Wrap);
			PTexture2DGroup textureGroup5 = fnLoadModelTexture(model, "/3D/Textures/tex5.png", sTextureFolder + "text5.png", eTextureType::PNG, eTextureTileStyle::Wrap, eTextureTileStyle::Clamp);
			PTexture2DGroup textureGroup6 = fnLoadModelTexture(model, "/3D/Textures/tex6.png", sTextureFolder + "text6.png", eTextureType::PNG, eTextureTileStyle::Clamp, eTextureTileStyle::Mirror);


			// Side 1
			meshObject->SetTriangleProperties(0, fnCreateTexture(textureGroup1, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
			meshObject->SetTriangleProperties(1, fnCreateTexture(textureGroup1, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));

			// Side 2
			meshObject->SetTriangleProperties(2, fnCreateTexture(textureGroup2, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
			meshObject->SetTriangleProperties(3, fnCreateTexture(textureGroup2, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));

			// Side 3
			// Go outside of bounds on this side
			meshObject->SetTriangleProperties(4, fnCreateTexture(textureGroup3, -1.0, -1.0, 2.0, -1.0, 2.0, 2.0));
			meshObject->SetTriangleProperties(5, fnCreateTexture(textureGroup3, 2.0, 2.0, -1.0, 2.0, -1.0, -1.0));

			// Side 4
			meshObject->SetTriangleProperties(6, fnCreateTexture(textureGroup4, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
			meshObject->SetTriangleProperties(7, fnCreateTexture(textureGroup4, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));

			// Side 5
			meshObject->SetTriangleProperties(8, fnCreateTexture(textureGroup5, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
			meshObject->SetTriangleProperties(9, fnCreateTexture(textureGroup5, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));

			// Side 6
			meshObject->SetTriangleProperties(10, fnCreateTexture(textureGroup6, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
			meshObject->SetTriangleProperties(11, fnCreateTexture(textureGroup6, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));

			// Object Level Property
			meshObject->SetObjectLevelProperty(textureGroup1->GetResourceID(), 1);

			// Add build item
			model->AddBuildItem(meshObject.get(), wrapper->GetIdentityTransform());

			PWriter writer = model->QueryWriter("3mf");
			//writer->WriteToFile("texturegroup.3mf");
			writer->WriteToFile(sFileName);


			std::cout << "done" << std::endl;
		}

		void C3MFLoader::Save3mfFile(std::string sFileName, std::vector<mip::MeshTopology*>& meshList, std::vector<std::string>& meshNamelist, float fScale)
		{
			PWrapper wrapper = CWrapper::loadLibrary();
			
			std::cout << "------------------------------------------------------------------" << std::endl;
			std::cout << "3MF Save" << std::endl;
			printVersion(wrapper);
			std::cout << "------------------------------------------------------------------" << std::endl;
			PModel model = wrapper->CreateModel();

			try
			{
				std::string sTextureFolder = STRING_MANAGER->m_strAppDataLocalPath.toUtf8().constData();
				sTextureFolder = sTextureFolder + "/";
				
				for (int ii = 0; ii < meshList.size(); ii++)
				{
					PMeshObject meshObject = model->AddMeshObject();
					meshObject->SetName("Medip_Object_" + std::to_string(ii));

					//create 3mf geometry value from meshtopology
					std::vector<sLib3MFPosition> vertices;
					std::vector<sLib3MFTriangle> triangles;

					Create3mfVertexFromTopology(meshList[ii], vertices, fScale);
					Create3mfTriFromTopology(meshList[ii], triangles);

					meshObject->SetGeometry(vertices, triangles);

					int UVsize = meshList[ii]->m_uv.size();
#if 1
					if (UVsize > 0)
					{
						//Write png from text pixel data
						std::string TempName = "temp_Write_3mf_";
						std::vector<mip::TEXTURE>& TexDataList = meshList[ii]->m_TextureDataList;
						std::vector<PTexture2DGroup> txtGroupList;
						for (int jj = 0; jj < TexDataList.size(); jj++)
						{
							int TextureDataIndex = meshList[ii]->m_textureID[jj];

							int width = TexDataList[jj].getWidth();
							int height = TexDataList[jj].getHeight();
							int CHANNEL_NUM = TexDataList[jj].getChannels();
							unsigned char* data = TexDataList[jj].getTextureData();

							std::string filename = TempName + std::to_string(ii) + std::to_string(jj) + ".png";
							std::remove((sTextureFolder + filename).c_str());

							std::string tempFilePath = (sTextureFolder + filename).c_str();

							stbi_flip_vertically_on_write(true);
							stbi_write_png(tempFilePath.c_str(), width, height, CHANNEL_NUM, data, width * CHANNEL_NUM);

							//sOPCPath를 "/3D/Texture/" + filename 에서 다른걸로 바꾸면 3d Builder에서 안열릴때가 있다.
							PTexture2DGroup textureGroup = fnLoadModelTexture(model, "/3D/Texture/" + filename, sTextureFolder + filename, eTextureType::PNG, eTextureTileStyle::Wrap, eTextureTileStyle::Wrap);
							txtGroupList.push_back(textureGroup);
						}
#if 1
						int TextureDataIndex = 1;
						for (int jj = 0; jj < UVsize; jj += 3)
						{
							TextureDataIndex = meshList[ii]->m_textureID[jj];
							std::string filename = sTextureFolder + TempName + std::to_string(TextureDataIndex - 1) + ".png";

							// tri edge UV
							double t_1_U = meshList[ii]->m_uv[jj].x;
							double t_1_V = meshList[ii]->m_uv[jj].y;
							double t_2_U = meshList[ii]->m_uv[jj + 1].x;
							double t_2_V = meshList[ii]->m_uv[jj + 1].y;
							double t_3_U = meshList[ii]->m_uv[jj + 2].x;
							double t_3_V = meshList[ii]->m_uv[jj + 2].y;

							int index = (jj / 3);
							meshObject->SetTriangleProperties(index, fnCreateTexture(txtGroupList[TextureDataIndex - 1], t_1_U, t_1_V, t_2_U, t_2_V, t_3_U, t_3_V));
						}
#endif
						//// Object Level Property
						meshObject->SetObjectLevelProperty(txtGroupList[TextureDataIndex - 1]->GetResourceID(), 1);
					}
					else
					{
						sLib3MFTriangleProperties sTriangleColor;
						mip::VECTOR4 color = meshList[ii]->getShaderCommon()->getColor();
						PColorGroup colorGroup = model->AddColorGroup();
						Lib3MF_uint32 idColor = colorGroup->AddColor(wrapper->RGBAToColor(color.x, color.y, color.z, 255));
						sTriangleColor = fnCreateTriangleColor(colorGroup, idColor, idColor, idColor);
						for (int ii = 0; ii < triangles.size(); ii++)
						{
							meshObject->SetTriangleProperties(ii, sTriangleColor);
						}

						//// Object Level Property
						meshObject->SetObjectLevelProperty(sTriangleColor.m_ResourceID, sTriangleColor.m_PropertyIDs[0]);
					}
#endif
					std::vector<sTriangleProperties> PropertiesArrayBuffer;
					meshObject->GetAllTriangleProperties(PropertiesArrayBuffer);

					Lib3MF::sTransform _3mfMat = wrapper->GetIdentityTransform();
					MATRIX44 MedipMat = meshList[ii]->getMatrix();

					for (int jj = 0; jj < 4; jj++)
					{
						for (int kk = 0; kk < 4; kk++)
						{
							if (kk == 4) continue; _3mfMat.m_Fields[jj][kk] = MedipMat.m[jj][kk];
						}
					}

					//// Add build item
					PBuildItem pBuilditem = model->AddBuildItem(meshObject.get(), _3mfMat);
					pBuilditem->SetObjectTransform(_3mfMat);
				}

				PWriter writer = model->QueryWriter("3mf");
				writer->WriteToFile(sFileName);

				printf_s("\n 3MF Save3mfFile done \n");
			}
			catch (ELib3MFException& exception)
			{
				printf_s("\n 3MF Error : %s \n", exception.what());
				return;
			}
		}

		bool C3MFLoader::freeStbi_Data(unsigned char * data)
		{
			stbi_image_free(data);
			return true;
		}

		void C3MFLoader::update3mfVertex(mip::MeshTopology * pMT)
		{
			muint32 n_tris = pMT->m_ttris.size();
			muint32	cnt_visible = 0;
			std::vector<muint32> vt_tris_idx;
			vt_tris_idx.reserve(n_tris);
			for (muint32 ti = 0; ti < n_tris; ti++)
			{
				if (pMT->m_ttris[ti].isD() || pMT->m_ttris[ti].isUS1())
				{
					continue;
				}

				cnt_visible++;

				vt_tris_idx.push_back(ti);
			}

			vt_tris_idx.resize(cnt_visible);

			int n_verts = cnt_visible * 3;
			pMT->m_verts.resize(n_verts);
			pMT->m_normals.resize(n_verts);
			pMT->m_tris.resize(n_verts, -1);

			const auto		num_core = std::thread::hardware_concurrency();
			int				n_core = (int)num_core;

			concurrency::parallel_for(0, n_core, [&](int ii)
			{
				int interval = cnt_visible / n_core;
				int j = interval * ii;
				int finish = j + interval;
				int remain = 0;

				if (ii == n_core - 1)
				{
					remain = cnt_visible % n_core;
					finish += remain;
				}

				for (; j < finish; ++j)
				{
					int ti = vt_tris_idx[j];
					int pos0 = j * 3;
					int pos1 = pos0 + 1;
					int pos2 = pos0 + 2;

					auto & v1 = pMT->m_ttris[ti].vi[0];
					auto & v2 = pMT->m_ttris[ti].vi[1];
					auto & v3 = pMT->m_ttris[ti].vi[2];

					pMT->m_verts[pos0] = pMT->m_tverts[v1].pos;
					pMT->m_verts[pos1] = pMT->m_tverts[v2].pos;
					pMT->m_verts[pos2] = pMT->m_tverts[v3].pos;

					for (int jj = 0; jj < TTri::TRI_SIZE; jj++)
					{
						auto idx = pMT->m_ttris[ti].vi[jj];
						int n_tis = pMT->m_tverts[idx].tis.size();

						auto & vt_tis = pMT->m_tverts[idx].tis;

						mip::VECTOR3 vertex_normal(0, 0, 0);
						mip::VECTOR3 normal(0, 0, 0);
						for (int kk = 0; kk < n_tis; ++kk)
						{
							if (pMT->m_ttris[vt_tis[kk]].isD()) continue;
							auto & vidx1 = pMT->m_ttris[vt_tis[kk]].vi[0];
							auto & vidx2 = pMT->m_ttris[vt_tis[kk]].vi[1];
							auto & vidx3 = pMT->m_ttris[vt_tis[kk]].vi[2];

							normal = (pMT->m_tverts[vidx2].pos - pMT->m_tverts[vidx1].pos).cross((pMT->m_tverts[vidx3].pos - pMT->m_tverts[vidx1].pos));
							vertex_normal += normal;
						}

						vertex_normal /= (float)n_tis;
						vertex_normal.normalize();

						pMT->m_normals[pos0 + jj] = vertex_normal;
					}

					pMT->m_tris[pos0] = pos0;
					pMT->m_tris[pos1] = pos1;
					pMT->m_tris[pos2] = pos2;
				}
			});
		}

		bool C3MFLoader::Load3mfFile(std::string sFileName, std::vector<mip::MeshTopology*>& out, float fScale)
		{
			PWrapper wrapper = CWrapper::loadLibrary();

			std::cout << "------------------------------------------------------------------" << std::endl;
			std::cout << "3MF Texture Cube example" << std::endl;
			printVersion(wrapper);
			std::cout << "------------------------------------------------------------------" << std::endl;

			PModel model = wrapper->CreateModel();

			try
			{
				// Import Model from 3MF File
				{
					PReader reader = model->QueryReader("3mf");
					// And deactivate the strict mode (default is "false", anyway. This just demonstrates where/how to use it).
					reader->SetStrictModeActive(false);
					reader->ReadFromFile(sFileName);

					for (Lib3MF_uint32 iWarning = 0; iWarning < reader->GetWarningCount(); iWarning++) {
						Lib3MF_uint32 nErrorCode;
						std::string sWarningMessage = reader->GetWarning(iWarning, nErrorCode);
						std::cout << "Encountered warning #" << nErrorCode << " : " << sWarningMessage << std::endl;
					}
				}
				ShowMetaDataInformation(model->GetMetaDataGroup());

				PSliceStackIterator sliceStacks = model->GetSliceStacks();
				while (sliceStacks->MoveNext()) {
					PSliceStack sliceStack = sliceStacks->GetCurrentSliceStack();
					ShowSliceStack(sliceStack, "");
				}

				//
				PObjectIterator objectIterator					= model->GetObjects();
				PMeshObjectIterator MeshObjIterator				= model->GetMeshObjects();
				PTexture2DGroupIterator Texture2DGroupsIterator = model->GetTexture2DGroups();
				PComponentsObjectIterator CompObj				= model->GetComponentsObjects();

#if 1
				PObjectIterator objectIterator1 = model->GetObjects();
				while (objectIterator1->MoveNext()) {
					PObject object = objectIterator1->GetCurrentObject();
					if (object->IsMeshObject()) {
						ShowMeshObjectInformation(model->GetMeshObjectByID(object->GetResourceID()));
						//printf_s("   Texture Cnt:     %d\n", model->GetTexture2DGroups()->Count());
					}
					else if (object->IsComponentsObject()) {
						ShowComponentsObjectInformation(model->GetComponentsObjectByID(object->GetResourceID()));
					}
					else {
						//std::cout << "unknown object #" << object->GetResourceID() << ": " << std::endl;
					}
				}
#endif

				printf_s("\n----------------------------\n");

				//print information
				PBuildItemIterator buildItemIteratorPrint = model->GetBuildItems();
				int BuildCntPrint = buildItemIteratorPrint->Count();
				while (buildItemIteratorPrint->MoveNext()) {
					PBuildItem buildItem = buildItemIteratorPrint->GetCurrent();

					std::cout << "Build item (Object #" << buildItem->GetObjectResourceID() << "): " << std::endl;
					if (buildItem->HasObjectTransform()) {
						ShowTransform(buildItem->GetObjectTransform(), "   ");
					}
					else {
						std::cout << "   Transformation:  none" << std::endl;
					}
					std::cout << "   Part number:     \"" << buildItem->GetPartNumber() << "\"" << std::endl;
					if (buildItem->GetMetaDataGroup()->GetMetaDataCount() > 0) {
						ShowMetaDataInformation(buildItem->GetMetaDataGroup());
					}
				}
				printf_s("\n----------------------------\n");
				std::cout << "Total Cnt:     \"" << BuildCntPrint << "\"" << std::endl;
				std::cout << "Total Texture Cnt:     \"" << model->GetAttachmentCount() << "\"" << std::endl;
				printf_s("----------------------------\n");
#if 1
				std::string sTextureFolder = STRING_MANAGER->m_strAppDataLocalPath.toUtf8().constData();
				sTextureFolder = sTextureFolder + "/";

				PBuildItemIterator buildItemIterator = model->GetBuildItems();

				DWORD start, end;
				start = ::GetTickCount();

				//Extract GeoMetry Data
				vector<Lib3MF_uint32> arrObjResId;
				while (buildItemIterator->MoveNext()) {
					PBuildItem buildItem = buildItemIterator->GetCurrent();
					Lib3MF_uint32 ObjectId = buildItem->GetObjectResourceID();

					std::vector<std::vector<sPosition>> VerticesBuffer;
					std::vector<std::vector<sTriangle>> IndicesBuffer;
					std::vector<std::vector<double>> UVCoord;
					std::vector<std::vector<int>> UVTextureId;
					std::vector<sTransform> tranformList;
					
					int meshObjectCnt = 0;
					int textureCnt = 0;

					PComponentsObject pCompObj;
					try {
						pCompObj = model->GetComponentsObjectByID(ObjectId);

						int sum = 0;
						int size = pCompObj->GetComponentCount();
						//printf_s("\n ObjectId[%d] Size - %d", ObjectId, size);
						for (int nIndex = 0; nIndex < pCompObj->GetComponentCount(); nIndex++) {
							PComponent component = pCompObj->GetComponent(nIndex);
							Lib3MF_uint32 _objResID = component->GetObjectResourceID();

							PComponentsObject _pCompObj;
							try {
								_pCompObj = model->GetComponentsObjectByID(_objResID);
							}
							catch (ELib3MFException& exception)
							{
								_pCompObj = pCompObj;
								printf_s("\n ComponentObject[%d] has no component", ObjectId);
							}

							int _size = _pCompObj->GetComponentCount();
							//printf_s("\n  ObjectId[%d]'s CompObjID '%d' Size - %d", ObjectId, _objResID, _size);
							for (Lib3MF_uint32 _nIndex = 0; _nIndex < _pCompObj->GetComponentCount(); _nIndex++) {
								PComponent	  _component = _pCompObj->GetComponent(_nIndex);
								Lib3MF_uint32 _MeshObjResID = _component->GetObjectResourceID();

								PMeshObject pMeshObj = model->GetMeshObjectByID(_MeshObjResID);
								
								Get_GeometryDataIn3mfMeshObject(model, pMeshObj, arrObjResId, meshObjectCnt, textureCnt,
									VerticesBuffer, IndicesBuffer, UVCoord, UVTextureId);

								if (component->HasTransform()) 
								{
									component->GetTransform();
								} 
							}

							sTransform matForm;
							if (buildItem->HasObjectTransform()) { 
								matForm = buildItem->GetObjectTransform(); 
								tranformList.push_back(matForm);
							}
						}
					}
					catch (ELib3MFException& exception)
					{
						printf_s("\n Object[%d] has no component", ObjectId);
						PMeshObject pMeshObj = model->GetMeshObjectByID(ObjectId);

						Get_GeometryDataIn3mfMeshObject(model, pMeshObj, arrObjResId, meshObjectCnt, textureCnt,
							VerticesBuffer, IndicesBuffer, UVCoord, UVTextureId);
						
						sTransform matForm;
						if (buildItem->HasObjectTransform()) { 
							matForm = buildItem->GetObjectTransform(); 
							tranformList.push_back(matForm);
						}
					}

					//Extract Texture Data
					if (VerticesBuffer.size() >= 1)
					{
						mip::MeshTopology * mesh = new mip::MeshTopology(g_Renderer);
						mesh->initShader(mip::SHADERTYPE::SHADER_3MF);
						Combine_3mfMeshObject(mesh, VerticesBuffer, IndicesBuffer, UVCoord, UVTextureId, fScale);
						out.push_back(mesh);
						
#if 0 
						//MATRIX44 MedipMat;
						if (tranformList.size() == 1)
						{
							mesh->zero();

							MATRIX44 MedipMat2;
							MedipMat2.identity();
							Lib3MF::sTransform _3mfMat = wrapper->GetIdentityTransform();

							for (int jj = 0; jj < 4; jj++)
							{
								for (int kk = 0; kk < 3; kk++)
								{
									if (kk == 3) continue; MedipMat2.m[jj][kk] = tranformList[0].m_Fields[jj][kk];
								}
							}

							MATRIX44 MedipMat_org = mesh->getMatrix();
							MATRIX44 MedipMat = mesh->getMatrix();
							MedipMat *= MedipMat2;
							for (int jj = 0; jj < 4; jj++)
							{
								for (int kk = 0; kk < 3; kk++)
								{
									if (kk == 3) continue; MedipMat.m[jj][kk] = tranformList[0].m_Fields[jj][kk];
								}
							}

							mesh->setScale(MedipMat.extractScale());
							mesh->setRotate(MedipMat.getQuaternion());
							mesh->setTranslate(MedipMat.getOrigin());
						}
#endif 

						PTexture2DGroupIterator pTex2DGroup = model->GetTexture2DGroups();
						int _2dGroupCnt = pTex2DGroup->Count();
						int CurCnt = 0;
						while (pTex2DGroup->MoveNext())
						{
							PTexture2DGroup cur_texture = pTex2DGroup->GetCurrentTexture2DGroup();
							Lib3MF_uint32 id = cur_texture->GetResourceID();
							bool bIsTexture = false;
							for (int ii = 0; ii < arrObjResId.size(); ii++)
							{
								if (arrObjResId[ii] == cur_texture->GetResourceID())  bIsTexture = true;
							}

							if (!bIsTexture) continue;

							PTexture2D text_2d = cur_texture->GetTexture2D();
							PAttachment attchment = text_2d->GetAttachment();

							int find = sFileName.rfind("/") + 1;
							string _filePath = sFileName.substr(0, find);
							string _fileName = sFileName.substr(find, sFileName.length() - find);

							int ext = _fileName.rfind(".3mf");
							int name = _fileName.rfind("/") + 1;
							string _3mfFileName = _fileName.substr(0, name);
							_3mfFileName += _fileName.substr(name, ext - name);

							std::string filename = sTextureFolder + _3mfFileName + "_" + std::to_string(CurCnt) + ".png";

							eTextureTileStyle eTileStyleU; eTextureTileStyle eTileStyleV;
							text_2d->GetTileStyleUV(eTileStyleU, eTileStyleV);

							attchment->WriteToFile(filename);
							int textureID = CreateTexture(filename, mesh, (int)eTileStyleU, (int)eTileStyleV);
							std::remove(filename.c_str());


							std::string key = "Texture_" + std::to_string(CurCnt + 1);
							if (textureID != -1)
							{
								mesh->addTextureId(textureID, key);
								CurCnt++;
							}
							else
							{
								printf_s("\n %s Texture Error \n", key.c_str());
							}
						}
					}

					//PWriter writer = model->QueryWriter("stl");
					//writer->WriteToFile(sFileName + ".stl");

					end = ::GetTickCount();
					double time = (double)(end - start) / 1000.;
					printf_s("\n Load3mfFile process : %lf\n", time);

					printf_s("\n --- [ 3mf Load finished! ] --- \n\n");
				}
			}
			catch (ELib3MFException& exception)
			{				
				printf_s("\n 3MF Error : %s \n", exception.what());
				return false;
			}

			return true;
#endif
		}

#if 0 //test
		bool C3MFLoader::Load3mfFile_test(std::string sFileName, std::vector<mip::MeshTopology*>& out, float fScale)
		{
			PWrapper wrapper = CWrapper::loadLibrary();

			std::cout << "------------------------------------------------------------------" << std::endl;
			std::cout << "3MF Texture Cube example" << std::endl;
			printVersion(wrapper);
			std::cout << "------------------------------------------------------------------" << std::endl;

			PModel model = wrapper->CreateModel();

			PModel model_temp = wrapper->CreateModel();

			try
			{
				// Import Model from 3MF File
				{
					PReader reader = model->QueryReader("3mf");
					// And deactivate the strict mode (default is "false", anyway. This just demonstrates where/how to use it).
					reader->SetStrictModeActive(false);
					reader->ReadFromFile(sFileName);

					PReader reader2 = model_temp->QueryReader("3mf");
					// And deactivate the strict mode (default is "false", anyway. This just demonstrates where/how to use it).
					reader2->SetStrictModeActive(false);
					reader2->ReadFromFile(sFileName);




					for (Lib3MF_uint32 iWarning = 0; iWarning < reader->GetWarningCount(); iWarning++) {
						Lib3MF_uint32 nErrorCode;
						std::string sWarningMessage = reader->GetWarning(iWarning, nErrorCode);
						std::cout << "Encountered warning #" << nErrorCode << " : " << sWarningMessage << std::endl;
					}
				}
				ShowMetaDataInformation(model->GetMetaDataGroup());

				PSliceStackIterator sliceStacks = model->GetSliceStacks();
				while (sliceStacks->MoveNext()) {
					PSliceStack sliceStack = sliceStacks->GetCurrentSliceStack();
					ShowSliceStack(sliceStack, "");
				}

				//
				PObjectIterator objectIterator = model->GetObjects();
				PMeshObjectIterator MeshObjIterator = model->GetMeshObjects();
				PTexture2DGroupIterator Texture2DGroupsIterator = model->GetTexture2DGroups();
				PComponentsObjectIterator CompObj = model->GetComponentsObjects();

#if 1
				PObjectIterator objectIterator1 = model->GetObjects();
				while (objectIterator1->MoveNext()) {
					PObject object = objectIterator1->GetCurrentObject();
					if (object->IsMeshObject()) {
						ShowMeshObjectInformation(model->GetMeshObjectByID(object->GetResourceID()));
						printf_s("   Texture Cnt:     %d\n", model->GetTexture2DGroups()->Count());
					}
					else if (object->IsComponentsObject()) {
						ShowComponentsObjectInformation(model->GetComponentsObjectByID(object->GetResourceID()));
					}
					else {
						std::cout << "unknown object #" << object->GetResourceID() << ": " << std::endl;
					}
				}
#endif

				printf_s("\n----------------------------\n");

				//print information
				PBuildItemIterator buildItemIteratorPrint = model->GetBuildItems();
				int BuildCntPrint = buildItemIteratorPrint->Count();
				while (buildItemIteratorPrint->MoveNext()) {
					PBuildItem buildItem = buildItemIteratorPrint->GetCurrent();

					std::cout << "Build item (Object #" << buildItem->GetObjectResourceID() << "): " << std::endl;
					if (buildItem->HasObjectTransform()) {
						ShowTransform(buildItem->GetObjectTransform(), "   ");
					}
					else {
						std::cout << "   Transformation:  none" << std::endl;
					}
					std::cout << "   Part number:     \"" << buildItem->GetPartNumber() << "\"" << std::endl;
					if (buildItem->GetMetaDataGroup()->GetMetaDataCount() > 0) {
						ShowMetaDataInformation(buildItem->GetMetaDataGroup());
					}
				}
				printf_s("\n----------------------------\n");
				std::cout << "Total Cnt:     \"" << BuildCntPrint << "\"" << std::endl;
				std::cout << "Total Texture Cnt:     \"" << model->GetAttachmentCount() << "\"" << std::endl;
				printf_s("----------------------------\n");
#if 1
				std::string sTextureFolder = STRING_MANAGER->m_strAppDataLocalPath.toUtf8().constData();
				sTextureFolder = sTextureFolder + "/";

				PBuildItemIterator buildItemIterator = model->GetBuildItems();

				DWORD start, end;
				start = ::GetTickCount();

				//Extract GeoMetry Data
				vector<Lib3MF_uint32> arrObjResId;
				while (buildItemIterator->MoveNext()) {
					PBuildItem buildItem = buildItemIterator->GetCurrent();
					Lib3MF_uint32 ObjectId = buildItem->GetObjectResourceID();

					std::vector<std::vector<sPosition>> VerticesBuffer;
					std::vector<std::vector<sTriangle>> IndicesBuffer;
					std::vector<std::vector<double>> UVCoord;
					std::vector<std::vector<int>> UVTextureId;
					int meshObjectCnt = 0;
					int textureCnt = 0;

					PComponentsObject pCompObj;
					try {
						pCompObj = model->GetComponentsObjectByID(ObjectId);

						int sum = 0;
						int size = pCompObj->GetComponentCount();
						//printf_s("\n ObjectId[%d] Size - %d", ObjectId, size);
						for (int nIndex = 0; nIndex < pCompObj->GetComponentCount(); nIndex++) {
							PComponent component = pCompObj->GetComponent(nIndex);
							Lib3MF_uint32 _objResID = component->GetObjectResourceID();

							//model_temp->AddBuildItem(component.get(), identity);
							PComponentsObject _pCompObj;
							try {
								_pCompObj = model->GetComponentsObjectByID(_objResID);
							}
							catch (ELib3MFException& exception)
							{
								_pCompObj = pCompObj;
								printf_s("\n ComponentObject[%d] has no component", ObjectId);
							}

							int _size = _pCompObj->GetComponentCount();
							//printf_s("\n  ObjectId[%d]'s CompObjID '%d' Size - %d", ObjectId, _objResID, _size);
							for (Lib3MF_uint32 _nIndex = 0; _nIndex < _pCompObj->GetComponentCount(); _nIndex++) {
								PComponent	  _component = _pCompObj->GetComponent(_nIndex);
								Lib3MF_uint32 _MeshObjResID = _component->GetObjectResourceID();

								std::string uuid = buildItem->GetPartNumber();

								PMeshObject pMeshObj = model->GetMeshObjectByID(_MeshObjResID);
								//bool test = false;
								//std::string uuid2 = pMeshObj->GetUUID(test);
								//std::string GetPartNumber = pMeshObj->GetPartNumber();
								//std::string GetName = pMeshObj->GetName();
								Lib3MF_uint32 UnqResID = pMeshObj->GetUniqueResourceID();
								//Lib3MF_uint32 ResID = pMeshObj->GetResourceID();
								Lib3MF_uint32 ModelResIdFromPMesh = pMeshObj->GetModelResourceID();

								PTexture2DGroupIterator pTex2DGroup = model->GetTexture2DGroups();
								
								bool isTexture = false;
								while (pTex2DGroup->MoveNext())
								{
									PTexture2DGroup cur_texture = pTex2DGroup->GetCurrentTexture2DGroup();
									if (cur_texture->GetCount() < 1) break;
									PTexture2D text_2d = cur_texture->GetTexture2D();
									int modelResID2 = cur_texture->GetModelResourceID();
									int urid2 = cur_texture->GetUniqueResourceID();
									int rid2 = cur_texture->GetResourceID();
									
									int modelResID = text_2d->GetModelResourceID();
									int urid = text_2d->GetUniqueResourceID();
									int rid = text_2d->GetResourceID();
									string dd = text_2d->GetAttachment()->GetRelationShipType();
									Lib3MFHandle pDd = text_2d->GetHandle();

									Lib3MFHandle pDd123 = buildItem->GetHandle();
									if (pDd == pDd123)
									{
										isTexture = true; break;
									}
								}


								if (isTexture == false)
								{
									PBuildItemIterator buildItemIterator_temp = model_temp->GetBuildItems();
									
									while (buildItemIterator_temp->MoveNext()) {
										PBuildItem buildItem_temp = buildItemIterator_temp->GetCurrent();
										if (!(buildItem->GetOutbox().m_MaxCoordinate[0] == buildItem_temp->GetOutbox().m_MaxCoordinate[0] &&
											buildItem->GetOutbox().m_MaxCoordinate[1] == buildItem_temp->GetOutbox().m_MaxCoordinate[1] && 
											buildItem->GetOutbox().m_MaxCoordinate[2] == buildItem_temp->GetOutbox().m_MaxCoordinate[2] && 
											buildItem->GetOutbox().m_MinCoordinate[0] == buildItem_temp->GetOutbox().m_MinCoordinate[0] &&
											buildItem->GetOutbox().m_MinCoordinate[1] == buildItem_temp->GetOutbox().m_MinCoordinate[1] &&
											buildItem->GetOutbox().m_MinCoordinate[2] == buildItem_temp->GetOutbox().m_MinCoordinate[2] ))
										{
											model_temp->RemoveBuildItem(buildItem_temp.get());
										}
									}
									PWriter writer = model_temp->QueryWriter("stl");
									writer->WriteToFile(sFileName + "Model" + ".stl");
								}


								try {
								}
								catch (ELib3MFException& exception){
								}

								Get_GeometryDataIn3mfMeshObject(model, pMeshObj, arrObjResId, meshObjectCnt, textureCnt,
									VerticesBuffer, IndicesBuffer, UVCoord, UVTextureId);

								printf_s("\n    ObjectId[%d]'s CompObjID '%d''s _MeshObjResID[%d] _ Index Size : %d", ObjectId, _objResID, _MeshObjResID, IndicesBuffer[meshObjectCnt - 1].size());
								if (component->HasTransform()) { component->GetTransform(); } //Local ??
							}

							sTransform matForm;
							if (buildItem->HasObjectTransform()) { matForm = buildItem->GetObjectTransform(); }
						}
						//for (int jj = 0; jj < IndicesBuffer.size(); jj++)
						//{
						//	printf_s("\n VerticesBuffer[%d] Size - %d", jj, VerticesBuffer[jj].size());
						//	sum += IndicesBuffer[jj].size();
						//}
						//printf_s("\n [VerticesBuffer All Size - %d]", sum);
					}
					catch (ELib3MFException& exception)
					{
						printf_s("\n Object[%d] has no component", ObjectId);
						PMeshObject pMeshObj = model->GetMeshObjectByID(ObjectId);

						Get_GeometryDataIn3mfMeshObject(model, pMeshObj, arrObjResId, meshObjectCnt, textureCnt,
							VerticesBuffer, IndicesBuffer, UVCoord, UVTextureId);

						//sTransform identity;
						//model_temp->AddBuildItem(pMeshObj.get(), identity);

						//test
						//int sum = 0;
						//for (int jj = 0; jj < IndicesBuffer.size(); jj++)
						//{
						//	printf_s("\n VerticesBuffer[%d] Size - %d", jj, VerticesBuffer[jj].size());
						//	sum += IndicesBuffer[jj].size();
						//}
						//printf_s("\n [VerticesBuffer All Size - %d]", sum);
						sTransform matForm;
						if (buildItem->HasObjectTransform()) { matForm = buildItem->GetObjectTransform(); }
					}

					
					//PWriter writer = model_temp->QueryWriter("stl");
					//writer->WriteToFile(sFileName + "Model" + ".stl");

					//Extract Texture Data
					if (VerticesBuffer.size() >= 1)
					{
						mip::MeshTopology * mesh = new mip::MeshTopology(g_Renderer);
						mesh->initShader(mip::SHADERTYPE::SHADER_3MF);
						Combine_3mfMeshObject(mesh, VerticesBuffer, IndicesBuffer, UVCoord, UVTextureId, fScale);
						out.push_back(mesh);

						PTexture2DGroupIterator pTex2DGroup = model->GetTexture2DGroups();
						int _2dGroupCnt = pTex2DGroup->Count();
						int CurCnt = 0;
						while (pTex2DGroup->MoveNext())
						{
							PTexture2DGroup cur_texture = pTex2DGroup->GetCurrentTexture2DGroup();
							Lib3MF_uint32 id = cur_texture->GetResourceID();
							bool bIsTexture = false;
							for (int ii = 0; ii < arrObjResId.size(); ii++)
							{
								if (arrObjResId[ii] == cur_texture->GetResourceID())  bIsTexture = true;
							}

							if (!bIsTexture) continue;

							PTexture2D text_2d = cur_texture->GetTexture2D();
							PAttachment attchment = text_2d->GetAttachment();

							int ddd1 = text_2d->GetUniqueResourceID();
							int ddd12 = text_2d->GetResourceID();
							int ddd123 = text_2d->GetModelResourceID();
							PTexture2D ddd = model->GetTexture2DByID(ddd1);
							std::string filename = sTextureFolder + "temp_3mf_" + std::to_string(CurCnt) + ".png";

							eTextureTileStyle eTileStyleU; eTextureTileStyle eTileStyleV;
							text_2d->GetTileStyleUV(eTileStyleU, eTileStyleV);

							attchment->WriteToFile(filename);
							int textureID = CreateTexture(filename, mesh, (int)eTileStyleU, (int)eTileStyleV);
							std::remove(filename.c_str());


							std::string key = "Texture_" + std::to_string(CurCnt + 1);
							if (textureID != -1)
							{
								mesh->addTextureId(textureID, key);
								CurCnt++;
							}
							else
							{
								printf_s("\n %s Texture Error \n", key.c_str());
							}
						}
					}

					//PWriter writer = model->QueryWriter("stl");
					//writer->WriteToFile(sFileName + ".stl");

					end = ::GetTickCount();
					double time = (double)(end - start) / 1000.;
					printf_s("\n Load3mfFile process : %lf\n", time);

					printf_s("\n --- [ 3mf Load finished! ] --- \n\n");
				}
			}
			catch (ELib3MFException& exception)
			{
				printf_s("\n 3MF Error : %s \n", exception.what());
				return false;
			}

			return true;
#endif
		}
#endif

		int C3MFLoader::CreateTexture(std::string sFileName, mip::MeshTopology * pMT, int tileStyle_U, int tileStyle_V)
		{
			if (pMT == nullptr) return -1;

			//stb_image
			stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
													// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.

			int width, height, nrChannels;
			unsigned char* data = stbi_load(sFileName.c_str(), &width, &height, &nrChannels, 0);
			
			int textId = -1;
			if (data != nullptr)
			{
				int tileStyle = 2; // Clamp
				if (tileStyle_U == 1 || tileStyle_V == 1)  tileStyle = 1; // Repeat
				//printf_s("\n %s - u : %d / v : %d", sFileName.c_str(), tileStyle_U, tileStyle_V);

				WIN_MANAGER->makeCurrent();
				textId = pMT->createTexture(data, width, height, nrChannels, tileStyle);
				WIN_MANAGER->doneCurrent();

				int find = sFileName.rfind("/") + 1;
				string _filePath = sFileName.substr(0, find);
				string _fileName = sFileName.substr(find, sFileName.length() - find);

				mip::TEXTURE mipTexture(_fileName, width, height, 0, nrChannels);
				mipTexture.setTextureData(data);

				pMT->m_TextureDataList.push_back(mipTexture);
				int ddd = 0;

			}
			else
			{
				printf_s("\n Failed to load texture!!!");
				return -1;
			}
			//int len = std::strlen((char*)data);
			//stbi_image_free(data);

			return textId;
		}

		int C3MFLoader::Combine_3mfMeshObject(mip::MeshTopology * pMT, std::vector<std::vector<sPosition>>& VerticesBuffer, std::vector<std::vector<sTriangle>>& IndicesBuffer, 
			std::vector<std::vector<double>>& UVCoord, std::vector<std::vector<int>>& UVTextureId, float scale)
		{
			//--------------------------------
			// Init
			//--------------------------------
			pMT->m_verts.clear(); int bufSzie = 0;
			for (int ii = 0; ii < VerticesBuffer.size(); ii++)
				bufSzie += VerticesBuffer[ii].size();
			pMT->m_verts.resize(bufSzie);

			pMT->m_tris.clear(); bufSzie = 0;
			for (int ii = 0; ii < IndicesBuffer.size(); ii++)
				bufSzie += IndicesBuffer[ii].size();
			pMT->m_tris.resize(bufSzie * 3);

			pMT->m_uv.clear(); bufSzie = 0;
			for (int ii = 0; ii < UVCoord.size(); ii++)
				bufSzie += (int)(UVCoord[ii].size() / 2);
			pMT->m_uv.resize(bufSzie);

			pMT->m_textureID.clear(); bufSzie = 0;
			for (int ii = 0; ii < UVTextureId.size(); ii++)
				bufSzie += UVTextureId[ii].size();
			pMT->m_textureID.resize(bufSzie);

			pMT->m_tverts.clear();
			pMT->m_ttris.clear();
			pMT->m_tVHedges.clear();

			//--------------------------------
			// Push 3mf Data to Mesh Topology
			//--------------------------------
			int iter = 0;
			for (int ii = 0; ii < VerticesBuffer.size(); ii++)
			{
				if (ii - 1 >= 0) iter += VerticesBuffer[ii - 1].size();
				for (int jj = 0; jj < VerticesBuffer[ii].size(); jj++)
				{
					float offset = scale;
					mip::VECTOR3 _v = mip::VECTOR3(VerticesBuffer[ii][jj].m_Coordinates[0],
						VerticesBuffer[ii][jj].m_Coordinates[1], VerticesBuffer[ii][jj].m_Coordinates[2]);
					pMT->m_verts[jj + iter] = _v * scale;
				}
			}

			iter = 0;
			for (int ii = 0; ii < IndicesBuffer.size(); ii++)
			{
				if (ii - 1 >= 0) iter += IndicesBuffer[ii - 1].size();
				for (int jj = 0; jj < IndicesBuffer[ii].size(); jj++)
				{
					pMT->m_tris[((jj + iter) * 3)] = IndicesBuffer[ii][jj].m_Indices[0];
					pMT->m_tris[((jj + iter) * 3) + 1] = IndicesBuffer[ii][jj].m_Indices[1];
					pMT->m_tris[((jj + iter) * 3) + 2] = IndicesBuffer[ii][jj].m_Indices[2];
				}
			}

			mergingVertexFrom3mf(pMT, pMT->m_verts, pMT->m_tris);

			std::vector<mip::VECTOR3> _verts;
			_verts.clear(); bufSzie = 0;
			_verts.resize(pMT->m_tris.size());
			for (int ii = 0; ii < _verts.size(); ii++)
			{
				int index = pMT->m_tris[ii];
				_verts[ii] = pMT->m_verts[index];
				pMT->m_tris[ii] = ii;
			}

			int _count = _verts.size();
			pMT->m_verts.reserve(_count);
			pMT->m_verts.assign(_verts.begin(), _verts.end());


			iter = 0;
			for (int ii = 0; ii < UVCoord.size(); ii++)
			{
				if (UVCoord[ii].size() % 2 != 0)
				{
					printf_s("\n --- [ 3mf UV Value Error! ] --- \n\n");
					return false;
				}

				if (ii - 1 >= 0)
					iter += UVCoord[ii - 1].size() == 0 ? 0 : (UVCoord[ii - 1].size() / 2);

				for (int jj = 0; jj < UVCoord[ii].size(); jj += 2)
				{
					int offset = 0; offset = (jj == 0 ? 1 : 2);
					pMT->m_uv[(jj / offset) + iter] = mip::VECTOR2(UVCoord[ii][jj], UVCoord[ii][jj + 1]);
				}
			}

			iter = 0;
			for (int ii = 0; ii < UVTextureId.size(); ii++)
			{
				if (ii - 1 >= 0) iter += UVTextureId[ii - 1].size();

				for (int jj = 0; jj < UVTextureId[ii].size(); jj++)
				{
					pMT->m_textureID[jj + iter] = UVTextureId[ii][jj];
				}
			}

			

			return 1;
		}

		int C3MFLoader::Get_GeometryDataIn3mfMeshObject(PModel model, PMeshObject pMeshObj, vector<Lib3MF_uint32>& listObjResId, int& InOut_meshObjCnt, int& InOut_TextureCnt,
			std::vector<std::vector<sPosition>>& VerticesBuffer, std::vector<std::vector<sTriangle>>& IndicesBuffer, std::vector<std::vector<double>>& UVCoord, std::vector<std::vector<int>>& UVTextureId)
		{

			//vertex, index
			std::vector<sPosition> _VerticesBuffer;
			std::vector<sTriangle> _IndicesBuffer;
			VerticesBuffer.push_back(_VerticesBuffer);
			IndicesBuffer.push_back(_IndicesBuffer);

			pMeshObj->GetVertices(VerticesBuffer[InOut_meshObjCnt]);
			pMeshObj->GetTriangleIndices(IndicesBuffer[InOut_meshObjCnt]);

			int cnt = pMeshObj->GetVertexCount();

			std::vector<sPosition> list;
			for (int ii = 0; ii < cnt; ii++)
			{
				sPosition dd;
				dd = pMeshObj->GetVertex(ii);
				list.push_back(dd);

			}

			if (model->GetAttachmentCount() < 1)
			{
				InOut_meshObjCnt++;
				return 2;
			}

			//Texture coord, Id
			std::vector<sTriangleProperties> _propertiesArrayBuffer;
			pMeshObj->GetAllTriangleProperties(_propertiesArrayBuffer);

			int allszie = _propertiesArrayBuffer.size();

			std::vector<int> _UVTextureId;
			std::vector<double> _UVCoord;
			
			_UVTextureId.clear();
			_UVTextureId.resize(allszie * 3, 0);
			_UVCoord.clear();
			_UVCoord.resize(allszie * 3 * 2, 0);
			
			UVTextureId.push_back(_UVTextureId);
			UVCoord.push_back(_UVCoord);
			
			int exceptionSize = 1;
			int txt_ResourceId = -1;
#if USE_PPL
			if (0)
			{
				const auto num_core = std::thread::hardware_concurrency();
				int n_core = 1;//
				if (num_core >= 3)
					n_core = 2;

				printf_s("\n USE PPL Core - %d", n_core);
					 
				int size = allszie;
				int interval = size / n_core;
				int remain = size % n_core;

				concurrency::parallel_for(0, n_core, [&](int i)
				{
					int start = i * interval;
					int finish = (i + 1) * interval;

					if (i == n_core - 1)
						finish += remain;

					for (int ii = start; ii < finish; ii++)
					{
					}
					printf_s("\n [%d] Core finished", i);
				});
			}
			else
#else
			{
				int TextureCount = -1;
				for (int ii = 0; ii < allszie; ii++)
				{
					//if(ii % 1000 == 0)
					//	printf_s("\n [%d] / [%d] finished", allszie, ii);

					int itRId = _propertiesArrayBuffer[ii].m_ResourceID;
					PTexture2DGroup group;
					sTex2Coord coord;
					if (GetTexture2DGroupByID(model, _propertiesArrayBuffer[ii].m_ResourceID, group) != 0)
					{
						for (int jj = 0; jj < 3; jj++)
						{
							int itorTextId = ((ii * 3) + jj);
							UVTextureId[InOut_meshObjCnt][itorTextId] = TextureCount;
						}

						continue;
					}


					if (txt_ResourceId != itRId)
					{
						bool bFind = false;
						for (int jj = 0; jj < listObjResId.size(); jj++)
						{
							if (itRId == listObjResId[jj])
							{
								txt_ResourceId = itRId;
								TextureCount = jj+1;
								bFind = true;
								break;
							}
						}
						if (!bFind)
						{
							listObjResId.push_back(itRId);
							txt_ResourceId = itRId;
							InOut_TextureCnt++;
							TextureCount = listObjResId.size();
						}
					}

					for (int jj = 0; jj < 3; jj++)
					{
						int uvId = _propertiesArrayBuffer[ii].m_PropertyIDs[jj];
						coord = group->GetTex2Coord(uvId);

						int itor = (ii * 3) * 2 + (jj * 2) + 0;
						int itorNext = (ii * 3) * 2 + (jj * 2) + 1;
						int itorTextId = ((ii * 3) + jj);

						UVCoord[InOut_meshObjCnt][itor] = coord.m_U;
						UVCoord[InOut_meshObjCnt][itorNext] = coord.m_V;
						UVTextureId[InOut_meshObjCnt][itorTextId] = TextureCount;
					}
				}
			}
#endif

			InOut_meshObjCnt++;
			return 1;
		}

		Lib3MF_uint32 C3MFLoader::GetTexture2DGroupByID(PModel model, const Lib3MF_uint32 nUniqueResourceID, PTexture2DGroup& pTex2DGroup)
		{	
			Lib3MFHandle hTexture2DGroupInstance = nullptr;
			Lib3MF_uint32 val = lib3mf_model_gettexture2dgroupbyid(model->GetHandle(), nUniqueResourceID, &hTexture2DGroupInstance);

			if (!hTexture2DGroupInstance) {
				return -1;
			}

			pTex2DGroup = std::make_shared<CTexture2DGroup>(model->GetWrapper(), hTexture2DGroupInstance);
			return 0;
		}

		void C3MFLoader::printVersion(PWrapper wrapper) {
			Lib3MF_uint32 nMajor, nMinor, nMicro;
			wrapper->GetLibraryVersion(nMajor, nMinor, nMicro);
			std::cout << "lib3mf version = " << nMajor << "." << nMinor << "." << nMicro;
			std::string sReleaseInfo, sBuildInfo;
			if (wrapper->GetPrereleaseInformation(sReleaseInfo)) {
				std::cout << "-" << sReleaseInfo;
			}
			if (wrapper->GetBuildInformation(sBuildInfo)) {
				std::cout << "+" << sBuildInfo;
			}
			std::cout << std::endl;
		}

		// Utility functions to create vertices and triangles
		sLib3MFPosition C3MFLoader::fnCreateVertex(float x, float y, float z)
		{
			sLib3MFPosition result;
			result.m_Coordinates[0] = x;
			result.m_Coordinates[1] = y;
			result.m_Coordinates[2] = z;
			return result;
		}

		sLib3MFTriangle C3MFLoader::fnCreateTriangle(int v0, int v1, int v2)
		{
			sLib3MFTriangle result;
			result.m_Indices[0] = v0;
			result.m_Indices[1] = v1;
			result.m_Indices[2] = v2;
			return result;
		}

		PTexture2DGroup C3MFLoader::fnLoadModelTexture(PModel model, const std::string sOPCPath, const std::string sFilePath, eLib3MFTextureType eType, eLib3MFTextureTileStyle eTileStyleU, eLib3MFTextureTileStyle eTileStyleV)
		{
			std::string sRelationshipType_Texture = "http://schemas.microsoft.com/3dmanufacturing/2013/01/3dtexture";
			PAttachment attachment = model->AddAttachment(sOPCPath, sRelationshipType_Texture);
			attachment->ReadFromFile(sFilePath);

			PTexture2D texture2D = model->AddTexture2DFromAttachment(attachment.get());

			texture2D->SetContentType(eType);
			texture2D->SetTileStyleUV(eTileStyleU, eTileStyleV);

			PTexture2DGroup textureGroup = model->AddTexture2DGroup(texture2D.get());
			return textureGroup;
		}

		sLib3MFTriangleProperties C3MFLoader::fnCreateTexture(PTexture2DGroup textureGroup, double u1, double v1, double u2, double v2, double u3, double v3)
		{
			sLib3MFTriangleProperties property;
			property.m_ResourceID = textureGroup->GetResourceID();
			property.m_PropertyIDs[0] = textureGroup->AddTex2Coord(sLib3MFTex2Coord({ u1, v1 }));
			property.m_PropertyIDs[1] = textureGroup->AddTex2Coord(sLib3MFTex2Coord({ u2, v2 }));
			property.m_PropertyIDs[2] = textureGroup->AddTex2Coord(sLib3MFTex2Coord({ u3, v3 }));
			return property;
		}

		sLib3MFTriangleProperties C3MFLoader::fnCreateTriangleColor(PColorGroup colorGroup, Lib3MF_uint32 colorID1, Lib3MF_uint32 colorID2, Lib3MF_uint32 colorID3)
		{
			sLib3MFTriangleProperties sTriangleProperty;
			sTriangleProperty.m_ResourceID = colorGroup->GetResourceID();
			sTriangleProperty.m_PropertyIDs[0] = colorID1;
			sTriangleProperty.m_PropertyIDs[1] = colorID2;
			sTriangleProperty.m_PropertyIDs[2] = colorID3;
			return sTriangleProperty;
		}


		// -------- show extrat

		void C3MFLoader::ShowMetaDataInformation(PMetaDataGroup metaDataGroup)
		{
			Lib3MF_uint32 nMetaDataCount = metaDataGroup->GetMetaDataCount();

			for (Lib3MF_uint32 iMeta = 0; iMeta < nMetaDataCount; iMeta++) {

				PMetaData metaData = metaDataGroup->GetMetaData(iMeta);
				std::string sMetaDataValue = metaData->GetValue();
				std::string sMetaDataName  = metaData->GetName();
				std::string sMetaDataKey   = metaData->GetKey();
				std::string sMetaDataType  = metaData->GetType();
				std::string sMetaDataNSpace = metaData->GetNameSpace();
				std::cout << "Metadatum: " << iMeta << ":" << std::endl;
				std::cout << "Name  = \"" << sMetaDataName << "\"" << std::endl;
				std::cout << "Key   = \"" << sMetaDataKey << "\"" << std::endl;
				std::cout << "Type  = \"" << sMetaDataType << "\"" << std::endl;
				std::cout << "Value = \"" << sMetaDataValue << "\"" << std::endl;
				std::cout << "Space = \"" << sMetaDataNSpace << "\"" << std::endl;
			}
		}

		void C3MFLoader::ShowSliceStack(PSliceStack sliceStack, std::string indent)
		{
			std::cout << indent << "SliceStackID:  " << sliceStack->GetResourceID() << std::endl;
			if (sliceStack->GetSliceCount() > 0) {
				std::cout << indent << "  Slice count:  " << sliceStack->GetSliceCount() << std::endl;
			}
			if (sliceStack->GetSliceRefCount() > 0) {
				std::cout << indent << "  Slice ref count:  " << sliceStack->GetSliceRefCount() << std::endl;
				for (Lib3MF_uint64 iSliceRef = 0; iSliceRef < sliceStack->GetSliceRefCount(); iSliceRef++) {
					std::cout << indent << "  Slice ref :  " << sliceStack->GetSliceStackReference(iSliceRef)->GetResourceID() << std::endl;
				}
			}
		}

		void C3MFLoader::ShowObjectProperties(PObject object)
		{
			std::cout << "   Name:            \"" << object->GetName() << "\"" << std::endl;
			std::cout << "   PartNumber:      \"" << object->GetPartNumber() << "\"" << std::endl;

			switch (object->GetType()) {
			case eObjectType::Model:
				std::cout << "   Object type:     model" << std::endl;
				break;
			case eObjectType::Support:
				std::cout << "   Object type:     support" << std::endl;
				break;
			case eObjectType::SolidSupport:
				std::cout << "   Object type:     solidsupport" << std::endl;
				break;
			case eObjectType::Other:
				std::cout << "   Object type:     other" << std::endl;
				break;
			default:
				std::cout << "   Object type:     invalid" << std::endl;
				break;
			}

			if (object->HasSlices(false)) {
				PSliceStack sliceStack = object->GetSliceStack();
				ShowSliceStack(sliceStack, "   ");
			}

			if (object->GetMetaDataGroup()->GetMetaDataCount() > 0) {
				ShowMetaDataInformation(object->GetMetaDataGroup());
			}
		}

		Lib3MF_uint32 C3MFLoader::ShowMeshObjectInformation(PMeshObject meshObject)
		{
			std::cout << "mesh object #" << meshObject->GetResourceID() << ": " << std::endl;

			ShowObjectProperties(meshObject);

			Lib3MF_uint64 nVertexCount = meshObject->GetVertexCount();
			Lib3MF_uint64 nTriangleCount = meshObject->GetTriangleCount();
			PBeamLattice beamLattice = meshObject->BeamLattice();

			std::vector<sTriangleProperties> PropertiesArrayBuffer;
			meshObject->GetAllTriangleProperties(PropertiesArrayBuffer);

			// Output data
			std::cout << "   Vertex count:    " << nVertexCount << std::endl;
			std::cout << "   Triangle count:  " << nTriangleCount << std::endl;

			Lib3MF_uint64 nBeamCount = beamLattice->GetBeamCount();
			if (nBeamCount > 0) {
				std::cout << "   Beam count:  " << nBeamCount << std::endl;
				Lib3MF_uint32 nRepresentationMesh;
				if (beamLattice->GetRepresentation(nRepresentationMesh))
					std::cout << "   |_Representation Mesh ID:  " << nRepresentationMesh << std::endl;
				eLib3MFBeamLatticeClipMode eClipMode;
				Lib3MF_uint32 nClippingMesh;
				beamLattice->GetClipping(eClipMode, nClippingMesh);
				if (eClipMode != eBeamLatticeClipMode::NoClipMode)
					std::cout << "   |_Clipping Mesh ID:  " << nClippingMesh << "(mode=" << (int)eClipMode << ")" << std::endl;
				if (beamLattice->GetBeamSetCount() > 0) {
					std::cout << "   |_BeamSet count:  " << beamLattice->GetBeamSetCount() << std::endl;
				}
			}

			return meshObject->GetResourceID();
		}

		void C3MFLoader::ShowTransform(sLib3MFTransform transform, std::string indent) {
			std::cout << indent << "Transformation:  [ " << transform.m_Fields[0][0] << " " << transform.m_Fields[1][0] << " " << transform.m_Fields[2][0] << " " << transform.m_Fields[3][0] << " ]" << std::endl;
			std::cout << indent << "                 [ " << transform.m_Fields[0][1] << " " << transform.m_Fields[1][1] << " " << transform.m_Fields[2][1] << " " << transform.m_Fields[3][1] << " ]" << std::endl;
			std::cout << indent << "                 [ " << transform.m_Fields[0][2] << " " << transform.m_Fields[1][2] << " " << transform.m_Fields[2][2] << " " << transform.m_Fields[3][2] << " ]" << std::endl;
		}

		void C3MFLoader::ShowComponentsObjectInformation(PComponentsObject componentsObject)
		{
			std::cout << "components object #" << componentsObject->GetResourceID() << ": " << std::endl;

			ShowObjectProperties(componentsObject);
			std::cout << "   Component count:    " << componentsObject->GetComponentCount() << std::endl;
			for (Lib3MF_uint32 nIndex = 0; nIndex < componentsObject->GetComponentCount(); nIndex++) {
				PComponent component = componentsObject->GetComponent(nIndex);

				std::cout << "   Component " << nIndex << ":    Object ID:   " << component->GetObjectResourceID() << std::endl;
				if (component->HasTransform()) {
					ShowTransform(component->GetTransform(), "                   ");
				}
				else {
					std::cout << "                   Transformation:  none" << std::endl;
				}
			}
		}

		int C3MFLoader::Create3mfVertexFromTopology(mip::MeshTopology * in_pMT, std::vector<sLib3MFPosition>& vertices, float fScale)
		{
			if (in_pMT == nullptr) return 0;

			vertices.clear();
			vertices.reserve(in_pMT->m_tverts.size());
			for (muint32 ii = 0; ii < in_pMT->m_tverts.size(); ii++)
			{
				if (in_pMT->m_tverts[ii].isD()) continue;

				sLib3MFPosition pt = fnCreateVertex(in_pMT->m_tverts[ii].pos.x * fScale, in_pMT->m_tverts[ii].pos.y * fScale, in_pMT->m_tverts[ii].pos.z * fScale);
				//vertices[ii] = pt;
				vertices.push_back(pt);
			}

			return 1;
		}

		int C3MFLoader::Create3mfTriFromTopology(mip::MeshTopology * in_pMT, std::vector<sLib3MFTriangle>& triangles)
		{
			if (in_pMT == nullptr) return 0;

			triangles.clear();
			triangles.reserve(in_pMT->m_ttris.size());
			for (muint32 ii = 0; ii < in_pMT->m_ttris.size(); ii++)
			{
				if (in_pMT->m_ttris[ii].isD())
					continue;

				int v1 = in_pMT->m_ttris[ii].vi[0];
				int v2 = in_pMT->m_ttris[ii].vi[1];
				int v3 = in_pMT->m_ttris[ii].vi[2];

				if (in_pMT->m_tverts[v1] == VECTOR3(0, 0, 0) && in_pMT->m_tverts[v2] == VECTOR3(0, 0, 0) && in_pMT->m_tverts[v3] == VECTOR3(0, 0, 0))
					continue;

				if (in_pMT->m_tverts[v1].isD() || in_pMT->m_tverts[v2].isD() || in_pMT->m_tverts[v3].isD() )
					continue;

				if (in_pMT->m_tverts.size() <= v1 || in_pMT->m_tverts.size() <= v2 || in_pMT->m_tverts.size() <= v3)
				{
					continue;
				}

				if (v1 == 0 || v2 == 0)
					int dsfsdf = 0;

				sLib3MFTriangle result = fnCreateTriangle(v1, v2, v3);
				triangles.push_back(result);
				//triangles[ii] = result;
			}

			return 1;
		}

		bool C3MFLoader::mergingVertexFrom3mf(mip::MeshTopology * pMT, std::vector<sLib3MFPosition>& vertices, std::vector<sLib3MFTriangle>& triangles)
		{
			pMT->m_tverts.clear();
			pMT->m_tverts.resize(vertices.size());

			pMT->m_ttris.clear(); 
			pMT->m_ttris.resize(triangles.size());

			AABB box;
			box.min.set(FLT_MAX, FLT_MAX, FLT_MAX);
			box.max.set(FLT_MIN, FLT_MIN, FLT_MIN);
			for (int ii = 0; ii < pMT->m_tverts.size(); ii++)
			{
				mip::VECTOR3 pos = mip::VECTOR3(vertices.at(ii).m_Coordinates[0], vertices.at(ii).m_Coordinates[1], vertices.at(ii).m_Coordinates[2]);
				TVert Vertex;
				Vertex.pos = pos;
				Vertex.vi = ii;
				pMT->m_tverts[ii].pos = pos;

				if (Vertex.pos.x > box.max.x)
					box.max.x = Vertex.pos.x;
				if (Vertex.pos.y > box.max.y)
					box.max.y = Vertex.pos.y;
				if (Vertex.pos.z > box.max.z)
					box.max.z = Vertex.pos.z;

				if (Vertex.pos.x < box.min.x)
					box.min.x = Vertex.pos.x;
				if (Vertex.pos.y < box.min.y)
					box.min.y = Vertex.pos.y;
				if (Vertex.pos.z < box.min.z)
					box.min.z = Vertex.pos.z;
			}

			pMT->m_boundingBox = box;

			for (int ii = 0; ii < pMT->m_ttris.size(); ii++)
			{
				pMT->m_ttris[ii].vi[0] = triangles.at(ii).m_Indices[0];
				pMT->m_ttris[ii].vi[1] = triangles.at(ii).m_Indices[1];
				pMT->m_ttris[ii].vi[2] = triangles.at(ii).m_Indices[2];
				pMT->m_ttris[ii].hei = pMT->m_ttris[ii].vi[1];
			}


			return true;
		}

		bool C3MFLoader::mergingVertexFrom3mf(mip::MeshTopology * pMT, std::vector<mip::VECTOR3> & vertices, std::vector<muint32>& triangles)
		{
			pMT->m_tverts.clear();
			pMT->m_tverts.resize(vertices.size());

			pMT->m_ttris.clear();
			pMT->m_ttris.resize(triangles.size() / 3);

			AABB box;
			box.min.set(FLT_MAX, FLT_MAX, FLT_MAX);
			box.max.set(FLT_MIN, FLT_MIN, FLT_MIN);
			for (int ii = 0; ii < pMT->m_tverts.size(); ii++)
			{
				mip::VECTOR3 pos = mip::VECTOR3(vertices.at(ii).x, vertices.at(ii).y, vertices.at(ii).z);
				TVert & Vertex = pMT->m_tverts[ii];
				pMT->m_tverts[ii].pos = pos;
				pMT->m_tverts[ii].vi = ii;

				if (Vertex.pos.x > box.max.x)
					box.max.x = Vertex.pos.x;
				if (Vertex.pos.y > box.max.y)
					box.max.y = Vertex.pos.y;
				if (Vertex.pos.z > box.max.z)
					box.max.z = Vertex.pos.z;

				if (Vertex.pos.x < box.min.x)
					box.min.x = Vertex.pos.x;
				if (Vertex.pos.y < box.min.y)
					box.min.y = Vertex.pos.y;
				if (Vertex.pos.z < box.min.z)
					box.min.z = Vertex.pos.z;
			}

			pMT->m_boundingBox = box;

			for (int ii = 0; ii < pMT->m_ttris.size(); ii++)
			{
				pMT->m_ttris[ii].vi[0] = triangles[(ii * 3) + 0];
				pMT->m_ttris[ii].vi[1] = triangles[(ii * 3) + 1];
				pMT->m_ttris[ii].vi[2] = triangles[(ii * 3) + 2];
				pMT->m_ttris[ii].hei = pMT->m_ttris[ii].vi[1];
			}

			return true;
		}

		//PTexture2DGroup CModel::GetTexture2DGroupByID2(const Lib3MF_uint32 nUniqueResourceID)
		//{
		//	Lib3MFHandle hTexture2DGroupInstance = nullptr;
		//	CheckError(lib3mf_model_gettexture2dgroupbyid(m_pHandle, nUniqueResourceID, &hTexture2DGroupInstance));

		//	if (!hTexture2DGroupInstance) {
		//		CheckError(LIB3MF_ERROR_INVALIDPARAM);
		//	}
		//	return std::make_shared<CTexture2DGroup>(m_pWrapper, hTexture2DGroupInstance);
		//}
	}
}
// 3mf test ---------- start

//
//void CubeExample() {
//	PWrapper wrapper = wrapper->loadLibrary();
//
//	std::cout << "------------------------------------------------------------------" << std::endl;
//	std::cout << "3MF Color Cube example" << std::endl;
//	printVersion(wrapper);
//	std::cout << "------------------------------------------------------------------" << std::endl;
//
//	PModel model = wrapper->CreateModel();
//
//	PMeshObject meshObject = model->AddMeshObject();
//	meshObject->SetName("Colored Box");
//
//	// Create mesh structure of a cube
//	std::vector<sLib3MFPosition> vertices(8);
//	std::vector<sLib3MFTriangle> triangles(12);
//
//	float fSizeX = 100.0f;
//	float fSizeY = 200.0f;
//	float fSizeZ = 300.0f;
//
//	// Manually create vertices
//	vertices[0] = fnCreateVertex(0.0f, 0.0f, 0.0f);
//	vertices[1] = fnCreateVertex(fSizeX, 0.0f, 0.0f);
//	vertices[2] = fnCreateVertex(fSizeX, fSizeY, 0.0f);
//	vertices[3] = fnCreateVertex(0.0f, fSizeY, 0.0f);
//	vertices[4] = fnCreateVertex(0.0f, 0.0f, fSizeZ);
//	vertices[5] = fnCreateVertex(fSizeX, 0.0f, fSizeZ);
//	vertices[6] = fnCreateVertex(fSizeX, fSizeY, fSizeZ);
//	vertices[7] = fnCreateVertex(0.0f, fSizeY, fSizeZ);
//
//	// Manually create triangles
//	triangles[0] = fnCreateTriangle(2, 1, 0);
//	triangles[1] = fnCreateTriangle(0, 3, 2);
//	triangles[2] = fnCreateTriangle(4, 5, 6);
//	triangles[3] = fnCreateTriangle(6, 7, 4);
//	triangles[4] = fnCreateTriangle(0, 1, 5);
//	triangles[5] = fnCreateTriangle(5, 4, 0);
//	triangles[6] = fnCreateTriangle(2, 3, 7);
//	triangles[7] = fnCreateTriangle(7, 6, 2);
//	triangles[8] = fnCreateTriangle(1, 2, 6);
//	triangles[9] = fnCreateTriangle(6, 5, 1);
//	triangles[10] = fnCreateTriangle(3, 0, 4);
//	triangles[11] = fnCreateTriangle(4, 7, 3);
//
//	meshObject->SetGeometry(vertices, triangles);
//
//	// define colors
//	PColorGroup colorGroup = model->AddColorGroup();
//	Lib3MF_uint32 idRed = colorGroup->AddColor(wrapper->RGBAToColor(255, 0, 0, 255));
//	Lib3MF_uint32 idGreen = colorGroup->AddColor(wrapper->RGBAToColor(0, 255, 0, 255));
//	Lib3MF_uint32 idBlue = colorGroup->AddColor(wrapper->RGBAToColor(0, 0, 255, 255));
//	Lib3MF_uint32 idOrange = colorGroup->AddColor(wrapper->RGBAToColor(255, 128, 0, 255));
//	Lib3MF_uint32 idYellow = colorGroup->AddColor(wrapper->RGBAToColor(255, 255, 0, 255));
//
//	sLib3MFTriangleProperties sTriangleColorRed = fnCreateTriangleColor(colorGroup, idRed, idRed, idRed);
//	sLib3MFTriangleProperties sTriangleColorGreen = fnCreateTriangleColor(colorGroup, idGreen, idGreen, idGreen);
//	sLib3MFTriangleProperties sTriangleColorBlue = fnCreateTriangleColor(colorGroup, idBlue, idBlue, idBlue);
//
//	sLib3MFTriangleProperties sTriangleColor1 = fnCreateTriangleColor(colorGroup, idOrange, idRed, idYellow);
//	sLib3MFTriangleProperties sTriangleColor2 = fnCreateTriangleColor(colorGroup, idYellow, idGreen, idOrange);
//
//	// One-colored Triangles
//	meshObject->SetTriangleProperties(0, sTriangleColorRed);
//	meshObject->SetTriangleProperties(1, sTriangleColorRed);
//	meshObject->SetTriangleProperties(2, sTriangleColorGreen);
//	meshObject->SetTriangleProperties(3, sTriangleColorGreen);
//	meshObject->SetTriangleProperties(4, sTriangleColorBlue);
//	meshObject->SetTriangleProperties(5, sTriangleColorBlue);
//	// Gradient-colored Triangles
//	meshObject->SetTriangleProperties(6, sTriangleColor1);
//	meshObject->SetTriangleProperties(7, sTriangleColor2);
//	meshObject->SetTriangleProperties(8, sTriangleColor1);
//	meshObject->SetTriangleProperties(9, sTriangleColor2);
//	meshObject->SetTriangleProperties(10, sTriangleColor1);
//	meshObject->SetTriangleProperties(11, sTriangleColor2);
//
//	// Object Level Property
//	meshObject->SetObjectLevelProperty(sTriangleColorRed.m_ResourceID, sTriangleColorRed.m_PropertyIDs[0]);
//
//	// Add build item
//	model->AddBuildItem(meshObject.get(), wrapper->GetIdentityTransform());
//
//	PWriter writer = model->QueryWriter("3mf");
//	writer->WriteToFile("colorcube.3mf");
//
//	std::cout << "done" << std::endl;
//}
//
//
//
//void TextureExample(std::string sFileName) {
//	PWrapper wrapper = CWrapper::loadLibrary();
//
//	std::cout << "------------------------------------------------------------------" << std::endl;
//	std::cout << "3MF Texture Cube example" << std::endl;
//	printVersion(wrapper);
//	std::cout << "------------------------------------------------------------------" << std::endl;
//
//	PModel model = wrapper->CreateModel();
//
//	PMeshObject meshObject = model->AddMeshObject();
//	meshObject->SetName("Textured Box");
//
//	// Create mesh structure of a cube
//	std::vector<sLib3MFPosition> vertices(8);
//	std::vector<sLib3MFTriangle> triangles(12);
//
//	float fSizeX = 100.0f;
//	float fSizeY = 100.0f;
//	float fSizeZ = 100.0f;
//
//	// Manually create vertices
//	vertices[0] = fnCreateVertex(0.0f, 0.0f, 0.0f);
//	vertices[1] = fnCreateVertex(fSizeX, 0.0f, 0.0f);
//	vertices[2] = fnCreateVertex(fSizeX, fSizeY, 0.0f);
//	vertices[3] = fnCreateVertex(0.0f, fSizeY, 0.0f);
//	vertices[4] = fnCreateVertex(0.0f, 0.0f, fSizeZ);
//	vertices[5] = fnCreateVertex(fSizeX, 0.0f, fSizeZ);
//	vertices[6] = fnCreateVertex(fSizeX, fSizeY, fSizeZ);
//	vertices[7] = fnCreateVertex(0.0f, fSizeY, fSizeZ);
//
//	// Manually create triangles
//	triangles[0] = fnCreateTriangle(2, 1, 0);
//	triangles[1] = fnCreateTriangle(0, 3, 2);
//	triangles[2] = fnCreateTriangle(4, 5, 6);
//	triangles[3] = fnCreateTriangle(6, 7, 4);
//	triangles[4] = fnCreateTriangle(0, 1, 5);
//	triangles[5] = fnCreateTriangle(5, 4, 0);
//	triangles[6] = fnCreateTriangle(2, 3, 7);
//	triangles[7] = fnCreateTriangle(7, 6, 2);
//	triangles[8] = fnCreateTriangle(1, 2, 6);
//	triangles[9] = fnCreateTriangle(6, 5, 1);
//	triangles[10] = fnCreateTriangle(3, 0, 4);
//	triangles[11] = fnCreateTriangle(4, 7, 3);
//
//	meshObject->SetGeometry(vertices, triangles);
//
//	std::string sTextureFolder = "D:/3D/Texture/";
//	// add textures to 3mf package
//	std::cout << "sTextureFolder=\"" << sTextureFolder << "\"\n";
//	PTexture2DGroup textureGroup1 = fnLoadModelTexture(model, "/3D/Textures/tex1.png", sTextureFolder + "tex1.png", eTextureType::PNG, eTextureTileStyle::Wrap, eTextureTileStyle::Wrap);
//	PTexture2DGroup textureGroup2 = fnLoadModelTexture(model, "/3D/Textures/tex2.png", sTextureFolder + "tex2.png", eTextureType::PNG, eTextureTileStyle::Mirror, eTextureTileStyle::Wrap);
//	PTexture2DGroup textureGroup3 = fnLoadModelTexture(model, "/3D/Textures/tex3.png", sTextureFolder + "tex3.png", eTextureType::PNG, eTextureTileStyle::Wrap, eTextureTileStyle::Mirror);
//	PTexture2DGroup textureGroup4 = fnLoadModelTexture(model, "/3D/Textures/tex4.png", sTextureFolder + "tex4.png", eTextureType::PNG, eTextureTileStyle::Clamp, eTextureTileStyle::Wrap);
//	PTexture2DGroup textureGroup5 = fnLoadModelTexture(model, "/3D/Textures/tex5.png", sTextureFolder + "tex5.png", eTextureType::PNG, eTextureTileStyle::Wrap, eTextureTileStyle::Clamp);
//	PTexture2DGroup textureGroup6 = fnLoadModelTexture(model, "/3D/Textures/tex6.png", sTextureFolder + "tex6.png", eTextureType::PNG, eTextureTileStyle::Clamp, eTextureTileStyle::Mirror);
//
//
//	// Side 1
//	meshObject->SetTriangleProperties(0, fnCreateTexture(textureGroup1, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
//	meshObject->SetTriangleProperties(1, fnCreateTexture(textureGroup1, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));
//
//	// Side 2
//	meshObject->SetTriangleProperties(2, fnCreateTexture(textureGroup2, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
//	meshObject->SetTriangleProperties(3, fnCreateTexture(textureGroup2, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));
//
//	// Side 3
//	// Go outside of bounds on this side
//	meshObject->SetTriangleProperties(4, fnCreateTexture(textureGroup3, -1.0, -1.0, 2.0, -1.0, 2.0, 2.0));
//	meshObject->SetTriangleProperties(5, fnCreateTexture(textureGroup3, 2.0, 2.0, -1.0, 2.0, -1.0, -1.0));
//
//	// Side 4
//	meshObject->SetTriangleProperties(6, fnCreateTexture(textureGroup4, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
//	meshObject->SetTriangleProperties(7, fnCreateTexture(textureGroup4, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));
//
//	// Side 5
//	meshObject->SetTriangleProperties(8, fnCreateTexture(textureGroup5, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
//	meshObject->SetTriangleProperties(9, fnCreateTexture(textureGroup5, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));
//
//	// Side 6
//	meshObject->SetTriangleProperties(10, fnCreateTexture(textureGroup6, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0));
//	meshObject->SetTriangleProperties(11, fnCreateTexture(textureGroup6, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0));
//
//
//	int size = meshObject->GetTriangleCount();
//
//	for (int ii = 0; ii < size; ii++)
//	{
//		sTriangleProperties prop;
//		meshObject->GetTriangleProperties(ii, prop);
//		printf_s("\n tri - %d / ResourceID - %d / m_PropertyIDs - %d, %d, %d", ii, prop.m_ResourceID, prop.m_PropertyIDs[0], prop.m_PropertyIDs[1], prop.m_PropertyIDs[2]);
//		//printf_s("\n ResourceID - %d / [U1_%.4f , V1_%.4f] / [U2_%.4f , V2_%.4f] / [U3_%.4f , V3_%.4f]", prop.m_ResourceID, prop.m_PropertyIDs[0], v1, u2, v2, u3, v3);
//	}
//	std::vector<sTriangleProperties> PropertiesArrayBuffer5;
//	meshObject->GetAllTriangleProperties(PropertiesArrayBuffer5);
//
//	int allszie = PropertiesArrayBuffer5.size();
//	for (int ii = 0; ii < allszie; ii++)
//	{
//		int Id0 = PropertiesArrayBuffer5[ii].m_PropertyIDs[0];
//		int Id1 = PropertiesArrayBuffer5[ii].m_PropertyIDs[1];
//		int Id2 = PropertiesArrayBuffer5[ii].m_PropertyIDs[2];
//		int RId = PropertiesArrayBuffer5[ii].m_ResourceID;
//		PTexture2DGroup group = model->GetTexture2DGroupByID(PropertiesArrayBuffer5[ii].m_ResourceID);
//		int cnt = group->GetCount();
//		sTex2Coord coord0 = group->GetTex2Coord(Id0);
//		sTex2Coord coord1 = group->GetTex2Coord(Id1);
//		sTex2Coord coord2 = group->GetTex2Coord(Id2);
//
//		printf_s("\n ResourceID - %d / [U1_%.4f , V1_%.4f] / [U2_%.4f , V2_%.4f] / [U3_%.4f , V3_%.4f]",
//			RId, coord0.m_U, coord0.m_V, coord1.m_U, coord1.m_V, coord2.m_U, coord2.m_V);
//	}
//
//
//
//
//
//	std::vector<sTriangleProperties> PropertiesArrayBuffer2;
//	meshObject->GetAllTriangleProperties(PropertiesArrayBuffer2);
//
//	// Object Level Property
//	meshObject->SetObjectLevelProperty(textureGroup1->GetResourceID(), 1);
//
//	std::vector<sTriangleProperties> PropertiesArrayBuffer3;
//	meshObject->GetAllTriangleProperties(PropertiesArrayBuffer3);
//
//	// Add build item
//	model->AddBuildItem(meshObject.get(), wrapper->GetIdentityTransform());
//
//	PWriter writer = model->QueryWriter("3mf");
//	//writer->WriteToFile("texturegroup.3mf");
//	writer->WriteToFile(sFileName);
//
//
//	std::cout << "done" << std::endl;
//}
//

//void ExtractInfoExample(std::string sFileName) {
//	PWrapper wrapper = CWrapper::loadLibrary();
//
//	std::cout << "" << std::endl;
//	std::cout << "------------------------------------------------------------------" << std::endl;
//	std::cout << "3MF Read example" << std::endl;
//	printVersion(wrapper);
//	std::cout << "------------------------------------------------------------------" << std::endl;
//
//	PModel model = wrapper->CreateModel();
//
//	// Import Model from 3MF File
//	{
//		PReader reader = model->QueryReader("3mf");
//		// And deactivate the strict mode (default is "false", anyway. This just demonstrates where/how to use it).
//		reader->SetStrictModeActive(false);
//		reader->ReadFromFile(sFileName);
//
//		for (Lib3MF_uint32 iWarning = 0; iWarning < reader->GetWarningCount(); iWarning++) {
//			Lib3MF_uint32 nErrorCode;
//			std::string sWarningMessage = reader->GetWarning(iWarning, nErrorCode);
//			std::cout << "Encountered warning #" << nErrorCode << " : " << sWarningMessage << std::endl;
//		}
//	}
//	//ShowThumbnailInformation(model);
//
//	ShowMetaDataInformation(model->GetMetaDataGroup());
//
//
//
//	PSliceStackIterator sliceStacks = model->GetSliceStacks();
//	while (sliceStacks->MoveNext()) {
//		PSliceStack sliceStack = sliceStacks->GetCurrentSliceStack();
//		ShowSliceStack(sliceStack, "");
//	}
//
//	PObjectIterator objectIterator = model->GetObjects();
//	PMeshObjectIterator MeshObjIterator = model->GetMeshObjects();
//	while (objectIterator->MoveNext()) {
//		PObject object = objectIterator->GetCurrentObject();
//		if (object->IsMeshObject()) {
//			std::vector<Lib3MF_uint32> vt_id = ShowMeshObjectInformation(model->GetMeshObjectByID(object->GetResourceID()));
//
//			PTexture2DGroupIterator pTex2DGroup = model->GetTexture2DGroups();
//			int _2dGroupCnt = 0;
//			while (pTex2DGroup->MoveNext())
//			{
//				_2dGroupCnt++;
//				PTexture2DGroup cur_texture = pTex2DGroup->GetCurrentTexture2DGroup();
//
//				int cnt2dGroup = cur_texture->GetCount();
//				//cur_texture->GetTex2Coord(vt_id[i]);
//
//				PTexture2D text_2d = cur_texture->GetTexture2D();
//
//				eTextureTileStyle u_style, v_style;
//				text_2d->GetTileStyleUV(u_style, v_style);
//
//				printf_s("TileStyle : [%d %d]\n", u_style, v_style);
//
//				PAttachment attchment = text_2d->GetAttachment();
//
//				std::vector<Lib3MF_uint8> vt_buffer;
//				attchment->WriteToBuffer(vt_buffer);
//
//				attchment->WriteToFile("test.png");
//
//				for (int i = 0; i < vt_buffer.size(); ++i)
//				{
//					char c = (char)vt_buffer[i];
//
//					std::cout << c;
//				}
//
//				for (int i = 0; i < vt_id.size(); ++i)
//				{
//					auto tex_coord = cur_texture->GetTex2Coord(vt_id[i]);
//
//					printf_s("%d >> [%lf %lf]\n", i, tex_coord.m_U, tex_coord.m_V);
//				}
//			}
//
//		}
//		else if (object->IsComponentsObject()) {
//			ShowComponentsObjectInformation(model->GetComponentsObjectByID(object->GetResourceID()));
//		}
//		else {
//			std::cout << "unknown object #" << object->GetResourceID() << ": " << std::endl;
//		}
//	}
//
//
//	PBuildItemIterator buildItemIterator = model->GetBuildItems();
//	while (buildItemIterator->MoveNext()) {
//		PBuildItem buildItem = buildItemIterator->GetCurrent();
//
//		std::cout << "Build item (Object #" << buildItem->GetObjectResourceID() << "): " << std::endl;
//
//		if (buildItem->HasObjectTransform()) {
//			ShowTransform(buildItem->GetObjectTransform(), "   ");
//		}
//		else {
//			std::cout << "   Transformation:  none" << std::endl;
//		}
//		std::cout << "   Part number:     \"" << buildItem->GetPartNumber() << "\"" << std::endl;
//		if (buildItem->GetMetaDataGroup()->GetMetaDataCount() > 0) {
//			ShowMetaDataInformation(buildItem->GetMetaDataGroup());
//		}
//	}
//
//	PWriter writer = model->QueryWriter("stl");
//	writer->WriteToFile("test.stl");
//
//	std::cout << "done" << std::endl;
//	std::cout << "------------------------------------------------------------------" << std::endl;
//}
// 3mf test ---------- end
