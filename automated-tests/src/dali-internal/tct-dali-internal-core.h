#ifndef __TCT_DALI_INTERNAL_CORE_H__
#define __TCT_DALI_INTERNAL_CORE_H__

#include "testcase.h"

extern void utc_dali_material_startup(void);
extern void utc_dali_material_cleanup(void);

extern int UtcDaliFontMeasureTextWidth(void);
extern int UtcDaliFontMeasureTextWidthNegative(void);
extern int UtcDaliFontMeasureTextHeight(void);
extern int UtcDaliFontMeasureTextHeightNegative(void);
extern int UtcDaliCameraActorConstructorRefObject(void);
extern int UtcDaliImageActorConstructorRefObject(void);
extern int UtcDaliLightActorConstructorRefObject(void);
extern int UtcDaliMeshActorConstructorRefObject(void);
extern int UtcDaliModelConstructorRefObject(void);
extern int UtcDaliTextActorConstructorRefObject(void);
extern int UtcDaliImageFactoryUseCachedRequest01(void);
extern int UtcDaliImageFactoryUseCachedRequest02(void);
extern int UtcDaliImageFactoryUseCachedRequest03(void);
extern int UtcDaliImageFactoryUseCachedRequest04(void);
extern int UtcDaliImageFactoryCompatibleResource01(void);
extern int UtcDaliImageFactoryCompatibleResource02(void);
extern int UtcDaliImageFactoryCompatibleResource03(void);
extern int UtcDaliImageFactoryReload01(void);
extern int UtcDaliImageFactoryReload02(void);
extern int UtcDaliImageFactoryReload03(void);
extern int UtcDaliImageFactoryReload04(void);
extern int UtcDaliImageFactoryReload05(void);
extern int UtcDaliImageFactoryReload06(void);
extern int UtcDaliMaterialMethodNew01(void);
extern int UtcDaliMaterialMethodNew02(void);
extern int UtcDaliMaterialReadyTextureOffstage(void);
extern int UtcDaliMaterialUnreadyTextureOffstage(void);
extern int UtcDaliMaterialStaging01(void);
extern int UtcDaliMaterialStaging02(void);
extern int UtcDaliMaterialSetPropsWhilstStaged(void);
extern int UtcDaliMaterialSetTextureWhilstStaged(void);
extern int UtcDaliMaterialSetUnreadyTextureWhilstStaged(void);
extern int UtcDaliMaterialIsOpaqueWithoutTexture(void);
extern int UtcDaliMaterialIsOpaqueWithTexture(void);
extern int UtcDaliMaterialIsOpaqueWithProps(void);
extern int UtcDaliMaterialRender(void);
extern int UtcDaliTextGetImplementation(void);
extern int UtcDaliInternalRequestResourceBitmapRequests01(void);
extern int UtcDaliInternalRequestResourceBitmapRequests02(void);
extern int UtcDaliInternalRequestResourceBitmapRequests03(void);
extern int UtcDaliInternalRequestReloadBitmapRequests01(void);
extern int UtcDaliInternalRequestReloadBitmapRequests02(void);
extern int UtcDaliInternalRequestReloadBitmapRequests03(void);
extern int UtcDaliInternalSaveResource01(void);
extern int UtcDaliInternalSaveResource02(void);
extern int UtcDaliInternalSaveResource03(void);
extern int UtcDaliInternalSaveResource04(void);
extern int UtcDaliInternalSaveResource05(void);
extern int UtcDaliInternalRequestResourceTicket01(void);
extern int UtcDaliInternalRequestResourceTicket02(void);
extern int UtcDaliInternalLoadShaderRequest01(void);
extern int UtcDaliInternalLoadShaderRequest02(void);
extern int UtcDaliInternalAllocateBitmapImage01(void);
extern int UtcDaliInternalAddBitmapImage01(void);
extern int UtcDaliInternalAddBitmapImage02(void);
extern int UtcDaliInternalAddBitmapImage03(void);
extern int UtcDaliInternalGetBitmapImage01(void);
extern int UtcDaliInternalGetBitmapImage02(void);
extern int UtcDaliInternalGetBitmapImage03(void);
extern int UtcDaliInternalAllocateTexture01(void);
extern int UtcDaliInternalAddNativeImage(void);
extern int UtcDaliInternalAddFrameBufferImage(void);
extern int UtcDaliInternalAllocateMesh01(void);

testcase tc_array[] = {
    {"UtcDaliFontMeasureTextWidth", UtcDaliFontMeasureTextWidth, NULL, NULL},
    {"UtcDaliFontMeasureTextWidthNegative", UtcDaliFontMeasureTextWidthNegative, NULL, NULL},
    {"UtcDaliFontMeasureTextHeight", UtcDaliFontMeasureTextHeight, NULL, NULL},
    {"UtcDaliFontMeasureTextHeightNegative", UtcDaliFontMeasureTextHeightNegative, NULL, NULL},
    {"UtcDaliCameraActorConstructorRefObject", UtcDaliCameraActorConstructorRefObject, NULL, NULL},
    {"UtcDaliImageActorConstructorRefObject", UtcDaliImageActorConstructorRefObject, NULL, NULL},
    {"UtcDaliLightActorConstructorRefObject", UtcDaliLightActorConstructorRefObject, NULL, NULL},
    {"UtcDaliMeshActorConstructorRefObject", UtcDaliMeshActorConstructorRefObject, NULL, NULL},
    {"UtcDaliModelConstructorRefObject", UtcDaliModelConstructorRefObject, NULL, NULL},
    {"UtcDaliTextActorConstructorRefObject", UtcDaliTextActorConstructorRefObject, NULL, NULL},
    {"UtcDaliImageFactoryUseCachedRequest01", UtcDaliImageFactoryUseCachedRequest01, NULL, NULL},
    {"UtcDaliImageFactoryUseCachedRequest02", UtcDaliImageFactoryUseCachedRequest02, NULL, NULL},
    {"UtcDaliImageFactoryUseCachedRequest03", UtcDaliImageFactoryUseCachedRequest03, NULL, NULL},
    {"UtcDaliImageFactoryUseCachedRequest04", UtcDaliImageFactoryUseCachedRequest04, NULL, NULL},
    {"UtcDaliImageFactoryCompatibleResource01", UtcDaliImageFactoryCompatibleResource01, NULL, NULL},
    {"UtcDaliImageFactoryCompatibleResource02", UtcDaliImageFactoryCompatibleResource02, NULL, NULL},
    {"UtcDaliImageFactoryCompatibleResource03", UtcDaliImageFactoryCompatibleResource03, NULL, NULL},
    {"UtcDaliImageFactoryReload01", UtcDaliImageFactoryReload01, NULL, NULL},
    {"UtcDaliImageFactoryReload02", UtcDaliImageFactoryReload02, NULL, NULL},
    {"UtcDaliImageFactoryReload03", UtcDaliImageFactoryReload03, NULL, NULL},
    {"UtcDaliImageFactoryReload04", UtcDaliImageFactoryReload04, NULL, NULL},
    {"UtcDaliImageFactoryReload05", UtcDaliImageFactoryReload05, NULL, NULL},
    {"UtcDaliImageFactoryReload06", UtcDaliImageFactoryReload06, NULL, NULL},
    {"UtcDaliMaterialMethodNew01", UtcDaliMaterialMethodNew01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialMethodNew02", UtcDaliMaterialMethodNew02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialReadyTextureOffstage", UtcDaliMaterialReadyTextureOffstage, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialUnreadyTextureOffstage", UtcDaliMaterialUnreadyTextureOffstage, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialStaging01", UtcDaliMaterialStaging01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialStaging02", UtcDaliMaterialStaging02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialSetPropsWhilstStaged", UtcDaliMaterialSetPropsWhilstStaged, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialSetTextureWhilstStaged", UtcDaliMaterialSetTextureWhilstStaged, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialSetUnreadyTextureWhilstStaged", UtcDaliMaterialSetUnreadyTextureWhilstStaged, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialIsOpaqueWithoutTexture", UtcDaliMaterialIsOpaqueWithoutTexture, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialIsOpaqueWithTexture", UtcDaliMaterialIsOpaqueWithTexture, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialIsOpaqueWithProps", UtcDaliMaterialIsOpaqueWithProps, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialRender", UtcDaliMaterialRender, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliTextGetImplementation", UtcDaliTextGetImplementation, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalRequestResourceBitmapRequests01", UtcDaliInternalRequestResourceBitmapRequests01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalRequestResourceBitmapRequests02", UtcDaliInternalRequestResourceBitmapRequests02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalRequestResourceBitmapRequests03", UtcDaliInternalRequestResourceBitmapRequests03, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalRequestReloadBitmapRequests01", UtcDaliInternalRequestReloadBitmapRequests01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalRequestReloadBitmapRequests02", UtcDaliInternalRequestReloadBitmapRequests02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalRequestReloadBitmapRequests03", UtcDaliInternalRequestReloadBitmapRequests03, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalSaveResource01", UtcDaliInternalSaveResource01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalSaveResource02", UtcDaliInternalSaveResource02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalSaveResource03", UtcDaliInternalSaveResource03, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalSaveResource04", UtcDaliInternalSaveResource04, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalSaveResource05", UtcDaliInternalSaveResource05, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalRequestResourceTicket01", UtcDaliInternalRequestResourceTicket01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalRequestResourceTicket02", UtcDaliInternalRequestResourceTicket02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalLoadShaderRequest01", UtcDaliInternalLoadShaderRequest01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalLoadShaderRequest02", UtcDaliInternalLoadShaderRequest02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalAllocateBitmapImage01", UtcDaliInternalAllocateBitmapImage01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalAddBitmapImage01", UtcDaliInternalAddBitmapImage01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalAddBitmapImage02", UtcDaliInternalAddBitmapImage02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalAddBitmapImage03", UtcDaliInternalAddBitmapImage03, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalGetBitmapImage01", UtcDaliInternalGetBitmapImage01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalGetBitmapImage02", UtcDaliInternalGetBitmapImage02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalGetBitmapImage03", UtcDaliInternalGetBitmapImage03, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalAllocateTexture01", UtcDaliInternalAllocateTexture01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalAddNativeImage", UtcDaliInternalAddNativeImage, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalAddFrameBufferImage", UtcDaliInternalAddFrameBufferImage, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliInternalAllocateMesh01", UtcDaliInternalAllocateMesh01, utc_dali_material_startup, utc_dali_material_cleanup},
    {NULL, NULL}
};

#endif // __TCT_DALI_INTERNAL_CORE_H__
