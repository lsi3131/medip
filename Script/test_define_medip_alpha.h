#pragma once 

// 둘 중 하나만 선택
#define MEDIP_DEFAULT
#define DEEP_CATCH_VER			// MEDIP 일 경우 주석처리, 기능은 같지만 resoure, version 정보 다르게 포함됨

//meditim.dat 파일을 항상 압축하여 업데이트 하고 싶을 경우 사용.(DEBUG 모드일 경우에만 사용 가능)
//#define OVERWRITE_MEDITIM_DAT_FILE

#define ALPHA_VERSION				//ALPHA_VERSION은 경우. BETA_VERSION과 동일하다. 현재 기능 개발 중인 define을 추가한다.
#ifdef ALPHA_VERSION
//#define USE_UNIT_TEST
#define USE_MEDIP_MACRO
#else

#endif


#define BETA_VERSION
#ifndef BETA_VERSION	// 정식 버전인 경우(MEDIP_DEFAULT시 AI_VER define은 ai버전으로 빌드하는 경우 주석 해제 요망. 그 외에는 고정 사용 권장)
//	#define DEV_NO_UPDATE			// 개발디버깅용, 활성화 시 update안함
//	#define DEV_EXCEPTION_DUMP		// beta version 일 때만 사용, Mini-Dump On/Off	// release에 디버깅 정보 포함시 사용
#ifdef _DEBUG
	#define DEV_NO_LOG				// 개발디버깅용, 활성화 시 log 안 남김(debug 메시지로 남김)
#endif
//	#define AI_VER					// AI 기능 포함 시 활성화
	#define DEV_USE_APPDATA_PATH		// appdata path 사용에 대한 define. 사용자 배포시에만 오픈하여 배포.
#else					// beta 버전인 경우(beta build시 아래 define으로 고정 사용 권장 + release 프로젝트 속성에서 debugging 정보 포함 필요.)
	#define DEV_NO_UPDATE			// 개발디버깅용, 활성화 시 update안함
	#define DEV_EXCEPTION_DUMP		// beta version 일 때만 사용, Mini-Dump On/Off	// release에 디버깅 정보 포함시 사용
#ifdef _DEBUG
	#define DEV_NO_LOG				// 개발디버깅용, 활성화 시 log 안 남김(debug 메시지로 남김)
#endif
	#define AI_VER					// AI 기능 포함 시 활성화
	#define DEV_USE_APPDATA_PATH		// appdata path 사용에 대한 define. 사용자 배포시에만 오픈하여 배포.
#endif


#define MESH_TEST					// mesh editing 기능, 현재는 항상 활성화, 추후 삭제고려

// 안정화 될 때까지 define으로 관리
#define PET_CT					//PET-CT Overlay 사용시 활성화
#define DEV_FILE_MANAGER			//파일 관리자 사용시 활성화
#define SEGMENTATION_COLOR_CODING
//#define DEV_MENU_INTERFACE			//메뉴 인터페이스 사용시 활성화

////////////////////////////////////////////////////////////////////////////////////////
//#define OPEN_MIP_IN_DEEPCATCH
//#define OPEN_MIPD_IN_MEDIP // open mipd, save mip
//#define OPENCV_IMSHOW

////////////////////////////////////////////////////////////////////////////////////////
// 예전에 사용하던 것들, 코드 참고용으로 놔둠
//#define DEV_VER
//#define COVID19_VER // 코로나 버전, 현재 항상 기간제한 라이센스 들어감
//#define ONLY_FOR_FTP_UPLOAD_VER // 마스크저장x, mip만 저장 기능만 살려둠(fileopen, savefile, savemeshfile 에서 라이센스 체크 안함)

//#define AI_TRANSLATION
//#define MULTI_DRAWCUT_MODE

// const int g_Product_Version_DEEPCATCH 0x0004
#if defined DEEP_CATCH_VER
#define AI_VER
//#define DEEPCATCH_VER_MACRO // 정지봉 교수님 버전
#define DEEPCATCH_REPORT_150PPI_INTEGRATE					// deepcatch report 150ppi로 통합에 따른 작업 반영 define.(보완 작업 및 안정성 테스트 통과 후 풀 예정.)
//#define DEEPCATCH_REPORT_MASK_UID_USE						// deepatch report에서 사용하는 deepcatch class mask search시 uid 사용 방식 적용 define.(차후 mask 구조 개선 후 풀 예정.)
//#define QT_CHARTS_USE
#elif defined MEDIP_AI
#define AI_VER
#elif defined COVID19_VER
#define AI_VER

// const int g_Product_Version_COVID19 0x0010
#endif

#ifdef DEV_VER
#define	NEXT_VER //2.0.0.0
#endif

#ifdef AI_VER
#define TRAIN_PREVENT	// 모든 버전에서 train은 일단 빠지는 걸로, 학회나 추가 제품으로 개발 가능성 있음
#endif

//#define SUPPORT_PLANNING // 211214 허 건 과장 Planning    

#define USE_PRODUCT_MEDIP 1
#define USE_PRODUCT_DEEPCATCH 0
#define USE_ALPHA_VERSION 1
#define USE_BETA_VERSION 0

#if USE_PRODUCT_MEDIP
#undef DEEP_CATCH_VER			// MEDIP 일 경우 주석처리, 기능은 같지만 resoure, version 정보 다르게 포함됨
#elif USE_PRODUCT_DEEPCATCH
#undef MEDIP_DEFAULT
#endif

#if USE_ALPHA_VERSION
//DO Nothing
#elif USE_BETA_VERSION
#undef ALPHA_VERSION
#else
#undef ALPHA_VERSION
#undef BETA_VERSION
#endif
