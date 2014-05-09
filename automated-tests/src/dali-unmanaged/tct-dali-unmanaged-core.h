#ifndef __TCT_DALI_UNMANAGED_CORE_H__
#define __TCT_DALI_UNMANAGED_CORE_H__

#include "testcase.h"

extern void utc_dali_unmanaged_startup(void);
extern void utc_dali_unmanaged_cleanup(void);

extern int UtcDaliConstrainableDownCast(void);
extern int UtcDaliConstrainableDownCastNegative(void);
extern int UtcDaliConstrainableCustomProperty(void);
extern int UtcDaliContextVertexAttribStartup(void);
extern int UtcDaliContextVertexAttribImageRendering(void);
extern int UtcDaliContextVertexAttribImageAndModelRendering(void);
extern int UtcDaliCustomActorDoAction(void);
extern int UtcDaliGenerateDistanceField(void);
extern int UtcDaliDynamicsBodyConstructor(void);
extern int UtcDaliDynamicsBodyGetMass(void);
extern int UtcDaliDynamicsBodyGetElasticity(void);
extern int UtcDaliDynamicsBodySetLinearVelocity(void);
extern int UtcDaliDynamicsBodySetAngularVelocity(void);
extern int UtcDaliDynamicsBodySetKinematic(void);
extern int UtcDaliDynamicsBodyIsKinematic(void);
extern int UtcDaliDynamicsBodySetSleepEnabled(void);
extern int UtcDaliDynamicsBodyGetSleepEnabled(void);
extern int UtcDaliDynamicsBodyWakeUp(void);
extern int UtcDaliDynamicsBodyAddAnchor(void);
extern int UtcDaliDynamicsBodyConserveVolume(void);
extern int UtcDaliDynamicsBodyConserveShape(void);
extern int UtcDaliDynamicsBodyConfigNew(void);
extern int UtcDaliDynamicsBodyConfigConstructor(void);
extern int UtcDaliDynamicsBodyConfigType(void);
extern int UtcDaliDynamicsBodyConfigSetShape01(void);
extern int UtcDaliDynamicsBodyConfigSetShape02(void);
extern int UtcDaliDynamicsBodyConfigGetShape(void);
extern int UtcDaliDynamicsBodyConfigMass(void);
extern int UtcDaliDynamicsBodyConfigElasticity(void);
extern int UtcDaliDynamicsBodyConfigFriction(void);
extern int UtcDaliDynamicsBodyConfigLinearDamping(void);
extern int UtcDaliDynamicsBodyConfigAngularDamping(void);
extern int UtcDaliDynamicsBodyConfigLinearSleepVelocity(void);
extern int UtcDaliDynamicsBodyConfigAngularSleepVelocity(void);
extern int UtcDaliDynamicsBodyConfigCollisionGroup(void);
extern int UtcDaliDynamicsBodyConfigCollisionMask(void);
extern int UtcDaliDynamicsBodyConfigAnchorHardness(void);
extern int UtcDaliDynamicsBodyConfigVolumeConservation(void);
extern int UtcDaliDynamicsBodyConfigShapeConservation(void);
extern int UtcDaliDynamicsJointConstructor(void);
extern int UtcDaliDynamicsJointLinearLimit(void);
extern int UtcDaliDynamicsJointAngularLimit(void);
extern int UtcDaliDynamicsJointEnableSpring(void);
extern int UtcDaliDynamicsJointSetSpringStiffness(void);
extern int UtcDaliDynamicsJointSetSpringCenterPoint(void);
extern int UtcDaliDynamicsJointEnableMotor(void);
extern int UtcDaliDynamicsJointSetMotorVelocity(void);
extern int UtcDaliDynamicsJointSetMotorForce(void);
extern int UtcDaliDynamicsJointGetActor(void);
extern int UtcDaliDynamicsShapeConstructor(void);
extern int UtcDaliDynamicsShapeNewCapsule(void);
extern int UtcDaliDynamicsShapeNewCone(void);
extern int UtcDaliDynamicsShapeNewCube(void);
extern int UtcDaliDynamicsShapeNewCylinder(void);
extern int UtcDaliDynamicsShapeNewMesh(void);
extern int UtcDaliDynamicsShapeNewSphere(void);
extern int UtcDaliDynamicsShapeGetType(void);
extern int UtcDaliStageInitializeDynamics(void);
extern int UtcDaliStageGetDynamicsWorld(void);
extern int UtcDaliStageTerminateDynamics(void);
extern int UtcDaliDynamicsWorldConstructor(void);
extern int UtcDaliDynamicsWorldGravity(void);
extern int UtcDaliDynamicsWorldDebugDrawMode(void);
extern int UtcDaliDynamicsWorldRootActor(void);
extern int UtcDaliDynamicsWorldConfigConstructor(void);
extern int UtcDaliDynamicsWorldConfigNew(void);
extern int UtcDaliDynamicsWorldConfigType(void);
extern int UtcDaliEncodedBufferImageNew01(void);
extern int UtcDaliEncodedBufferImageNew02(void);
extern int UtcDaliEncodedBufferImageNew03(void);
extern int UtcDaliHitTestAlgorithmWithFunctor(void);
extern int UtcDaliHitTestAlgorithmWithFunctorOnRenderTask(void);
extern int UtcDaliHitTestAlgorithmOrtho01(void);
extern int UtcDaliHitTestAlgorithmOrtho02(void);
extern int UtcDaliHitTestAlgorithmStencil(void);
extern int UtcDaliLightActorConstructorVoid(void);
extern int UtcDaliLightActorDestructor(void);
extern int UtcDaliLightActorNew(void);
extern int UtcDaliLightActorDownCast(void);
extern int UtcDaliLightActorDownCast2(void);
extern int UtcDaliLightActorSetGetLight(void);
extern int UtcDaliLightActorSetGetActive(void);
extern int UtcDaliLightActorMeshTest(void);
extern int UtcDaliLightActorDefaultProperties(void);
extern int UtcDaliLightActorPropertyIndices(void);
extern int UtcDaliLocklessBufferWriteRead01(void);
extern int UtcDaliLocklessBufferMultipleWrites01(void);
extern int UtcDaliLocklessBufferGetSize01(void);
extern int UtcDaliMeshActorNew02(void);
extern int UtcDaliMeshActorCreateNoMeshData(void);
extern int UtcDaliMeshActorCreateSetData01(void);
extern int UtcDaliMeshActorCreateSetData02(void);
extern int UtcDaliMeshActorCreateSetData03(void);
extern int UtcDaliMeshActorCreateSetData04(void);
extern int UtcDaliMeshActorDownCast(void);
extern int UtcDaliMeshActorDownCast2(void);
extern int UtcDaliMeshActorSetMaterial01(void);
extern int UtcDaliMeshActorSetMaterial01b(void);
extern int UtcDaliMeshActorSetMaterial02(void);
extern int UtcDaliMeshActorSetMaterial02b(void);
extern int UtcDaliMeshActorSetMaterial03(void);
extern int UtcDaliMeshActorSetMaterial03b(void);
extern int UtcDaliMeshActorGetMaterial01(void);
extern int UtcDaliMeshActorGetMaterial02(void);
extern int UtcDaliMeshActorSetLighting01(void);
extern int UtcDaliMeshActorSetLighting02(void);
extern int UtcDaliMeshActorBlend01(void);
extern int UtcDaliMeshActorBlend02(void);
extern int UtcDaliMeshActorBlend03(void);
extern int UtcDaliMeshActorBlend04(void);
extern int UtcDaliMeshActorBlend05(void);
extern int UtcDaliMeshActorBlend06(void);
extern int UtcDaliMeshActorBlend07(void);
extern int UtcDaliMeshActorBlend08(void);
extern int UtcDaliMeshActorBlend09(void);
extern int UtcDaliMeshActorBoneUpdate01(void);
extern int UtcDaliMeshActorIndices(void);
extern int UtcDaliMeshDataNew(void);
extern int UtcDaliMeshDataSetData(void);
extern int UtcDaliMeshDataAddToBoundingVolume(void);
extern int UtcDaliMeshDataBoundingBox(void);
extern int UtcDaliMeshDataGetVertexCount(void);
extern int UtcDaliMeshDataGetVertices(void);
extern int UtcDaliMeshDataGetFaceCount(void);
extern int UtcDaliMeshDataGetFaces(void);
extern int UtcDaliMeshDataTextureCoords(void);
extern int UtcDaliMeshDataNormals(void);
extern int UtcDaliMeshDataGetMaterial(void);
extern int UtcDaliMeshDataSetMaterial(void);
extern int UtcDaliMeshDataGetBoneCount(void);
extern int UtcDaliMeshDataGetBones(void);
extern int UtcDaliModelConstructorVoid(void);
extern int UtcDaliModelNew(void);
extern int UtcDaliModelDownCast(void);
extern int UtcDaliModelGetLoadingState01(void);
extern int UtcDaliModelGetLoadingState02(void);
extern int UtcDaliModelGetLoadingState03(void);
extern int UtcDaliModelNumberOfAnimations(void);
extern int UtcDaliModelActorFactory(void);
extern int UtcDaliModelActorFactoryTwoMesh(void);
extern int UtcDaliModelBuildAnimation01(void);
extern int UtcDaliModelBuildAnimation02(void);
extern int UtcDaliModelBuildAnimation03(void);
extern int UtcDaliModelBuildAnimation04(void);
extern int UtcDaliModelBuildAnimation05(void);
extern int UtcDaliModelBuildAnimation06(void);
extern int UtcDaliModelBuildAnimation07(void);
extern int UtcDaliModelBuildAnimation08(void);
extern int UtcDaliMouseWheelEventConstructor(void);
extern int UtcDaliMouseWheelEventIsShiftModifier(void);
extern int UtcDaliMouseWheelEventIsCtrlModifier(void);
extern int UtcDaliMouseWheelEventIsAltModifier(void);
extern int UtcDaliMouseWheelEventIsNotShiftModifier(void);
extern int UtcDaliMouseWheelEventIsNotCtrlModifier(void);
extern int UtcDaliMouseWheelEventIsNotAltModifier(void);
extern int UtcDaliMouseWheelEventANDModifer(void);
extern int UtcDaliMouseWheelEventORModifer(void);
extern int UtcDaliMouseWheelEventSignalling(void);
extern int UtcDaliObjectRegistryGet(void);
extern int UtcDaliObjectRegistrySignalActorCreated(void);
extern int UtcDaliObjectRegistrySignalCameraCreated(void);
extern int UtcDaliObjectRegistrySignalImageActorCreated(void);
extern int UtcDaliObjectRegistrySignalLayerCreated(void);
extern int UtcDaliObjectRegistrySignalLightActorCreated(void);
extern int UtcDaliObjectRegistrySignalMeshActorCreated(void);
extern int UtcDaliObjectRegistrySignalModelCreated(void);
extern int UtcDaliObjectRegistrySignalTextActorCreated(void);
extern int UtcDaliObjectRegistrySignalAnimationCreated(void);
extern int UtcDaliObjectRegistrySignalShaderEffectCreated(void);
extern int UtcDaliRandomRangeMethod(void);
extern int UtcDaliRandomAxisMethod(void);
extern int UtcDaliRandomChanceMethod(void);
extern int UtcDaliRenderTaskSetScreenToFrameBufferMappingActor(void);
extern int UtcDaliShaderEffectFromProperties01(void);
extern int UtcDaliShaderEffectFromProperties02(void);
extern int UtcDaliShaderEffectFromProperties03(void);
extern int UtcDaliScriptingGetColorMode(void);
extern int UtcDaliScriptingGetPositionInheritanceMode(void);
extern int UtcDaliScriptingGetDrawMode(void);
extern int UtcDaliScriptingGetAnchorConstant(void);
extern int UtcDaliScriptingNewImageNegative(void);
extern int UtcDaliScriptingNewImage(void);
extern int UtcDaliScriptingNewShaderEffect(void);
extern int UtcDaliScriptingNewActorNegative(void);
extern int UtcDaliScriptingNewActorProperties(void);
extern int UtcDaliScriptingNewActorChildren(void);
extern int UtcDaliScriptingCreatePropertyMapActor(void);
extern int UtcDaliScriptingCreatePropertyMapImage(void);
extern int UtcDaliScriptingGetEnumerationTemplates(void);
extern int UtcDaliSplineGetYFromMonotonicX(void);
extern int utcDaliSplineGetKnot01(void);
extern int utcDaliSplineGetKnot02(void);
extern int utcDaliSplineGetKnot03(void);
extern int utcDaliSplineGetInTangent01(void);
extern int utcDaliSplineGetInTangent02(void);
extern int utcDaliSplineGetInTangent03(void);
extern int utcDaliSplineGetOutTangent01(void);
extern int utcDaliSplineGetOutTangent02(void);
extern int utcDaliSplineGetOutTangent03(void);
extern int utcDaliSplineGenerateControlPoints01(void);
extern int utcDaliSplineGenerateControlPoints02(void);
extern int utcDaliSplineGenerateControlPoints03(void);
extern int UtcDaliSplineGetY01(void);
extern int UtcDaliSplineGetY02(void);
extern int UtcDaliSplineGetY02b(void);
extern int UtcDaliSplineGetY03(void);
extern int UtcDaliSplineGetY04(void);
extern int UtcDaliSplineGetY04b(void);
extern int UtcDaliSplineGetPoint01(void);
extern int UtcDaliSplineGetPoint02(void);
extern int UtcDaliSplineGetPoint03(void);
extern int UtcDaliSplineGetPoint04(void);
extern int UtcDaliSplineGetPoint05(void);
extern int UtcDaliSplineGetPoint06(void);
extern int UtcDaliTypeRegistryCreateDaliObjects(void);
extern int UtcDaliTypeRegistryActionViaBaseHandle(void);
extern int UtcDaliTypeRegistryNames(void);
extern int UtcDaliTypeRegistryNameEquivalence(void);
extern int UtcDaliTypeRegistryCustomActor(void);
extern int UtcDaliTypeRegistryCustomSignalFailure(void);
extern int UtcDaliTypeRegistryInitFunctions(void);
extern int UtcDaliTypeRegistryNameInitFunctions(void);
extern int UtcDaliPropertyRegistration(void);
extern int UtcDaliPropertyRegistrationIndexOutOfBounds(void);
extern int UtcDaliPropertyRegistrationFunctions(void);
extern int UtcDaliPropertyRegistrationAddSameIndex(void);
extern int UtcDaliPropertyRegistrationPropertyWritable(void);
extern int UtcDaliPropertyRegistrationPropertyAnimatable(void);
extern int UtcDaliPropertyRegistrationInvalidGetAndSet(void);
extern int UtcDaliLongPressGestureDetectorTypeRegistry(void);
extern int UtcDaliPanGestureDetectorTypeRegistry(void);
extern int UtcDaliPinchGestureDetectorTypeRegistry(void);
extern int UtcDaliTapGestureDetectorTypeRegistry(void);
extern int UtcDaliTypeRegistryNamedType(void);
extern int UtcDaliUtf8SequenceLength(void);
extern int UtcDaliEmptyVectorInt(void);
extern int UtcDaliVectorInt(void);
extern int UtcDaliVectorIntCopy(void);
extern int UtcDaliVectorIntResize(void);
extern int UtcDaliVectorIntErase(void);
extern int UtcDaliVectorDoubleRemove(void);
extern int UtcDaliVectorIntSwap(void);
extern int UtcDaliVectorIterate(void);
extern int UtcDaliVectorPair(void);
extern int UtcDaliVectorAsserts(void);
extern int UtcDaliVectorAcidTest(void);
extern int UtcDaliVectorComplex(void);

testcase tc_array[] = {
    {"UtcDaliConstrainableDownCast", UtcDaliConstrainableDownCast, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliConstrainableDownCastNegative", UtcDaliConstrainableDownCastNegative, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliConstrainableCustomProperty", UtcDaliConstrainableCustomProperty, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliContextVertexAttribStartup", UtcDaliContextVertexAttribStartup, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliContextVertexAttribImageRendering", UtcDaliContextVertexAttribImageRendering, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliContextVertexAttribImageAndModelRendering", UtcDaliContextVertexAttribImageAndModelRendering, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliCustomActorDoAction", UtcDaliCustomActorDoAction, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliGenerateDistanceField", UtcDaliGenerateDistanceField, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConstructor", UtcDaliDynamicsBodyConstructor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyGetMass", UtcDaliDynamicsBodyGetMass, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyGetElasticity", UtcDaliDynamicsBodyGetElasticity, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodySetLinearVelocity", UtcDaliDynamicsBodySetLinearVelocity, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodySetAngularVelocity", UtcDaliDynamicsBodySetAngularVelocity, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodySetKinematic", UtcDaliDynamicsBodySetKinematic, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyIsKinematic", UtcDaliDynamicsBodyIsKinematic, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodySetSleepEnabled", UtcDaliDynamicsBodySetSleepEnabled, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyGetSleepEnabled", UtcDaliDynamicsBodyGetSleepEnabled, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyWakeUp", UtcDaliDynamicsBodyWakeUp, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyAddAnchor", UtcDaliDynamicsBodyAddAnchor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConserveVolume", UtcDaliDynamicsBodyConserveVolume, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConserveShape", UtcDaliDynamicsBodyConserveShape, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigNew", UtcDaliDynamicsBodyConfigNew, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigConstructor", UtcDaliDynamicsBodyConfigConstructor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigType", UtcDaliDynamicsBodyConfigType, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigSetShape01", UtcDaliDynamicsBodyConfigSetShape01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigSetShape02", UtcDaliDynamicsBodyConfigSetShape02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigGetShape", UtcDaliDynamicsBodyConfigGetShape, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigMass", UtcDaliDynamicsBodyConfigMass, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigElasticity", UtcDaliDynamicsBodyConfigElasticity, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigFriction", UtcDaliDynamicsBodyConfigFriction, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigLinearDamping", UtcDaliDynamicsBodyConfigLinearDamping, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigAngularDamping", UtcDaliDynamicsBodyConfigAngularDamping, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigLinearSleepVelocity", UtcDaliDynamicsBodyConfigLinearSleepVelocity, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigAngularSleepVelocity", UtcDaliDynamicsBodyConfigAngularSleepVelocity, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigCollisionGroup", UtcDaliDynamicsBodyConfigCollisionGroup, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigCollisionMask", UtcDaliDynamicsBodyConfigCollisionMask, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigAnchorHardness", UtcDaliDynamicsBodyConfigAnchorHardness, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigVolumeConservation", UtcDaliDynamicsBodyConfigVolumeConservation, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsBodyConfigShapeConservation", UtcDaliDynamicsBodyConfigShapeConservation, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointConstructor", UtcDaliDynamicsJointConstructor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointLinearLimit", UtcDaliDynamicsJointLinearLimit, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointAngularLimit", UtcDaliDynamicsJointAngularLimit, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointEnableSpring", UtcDaliDynamicsJointEnableSpring, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointSetSpringStiffness", UtcDaliDynamicsJointSetSpringStiffness, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointSetSpringCenterPoint", UtcDaliDynamicsJointSetSpringCenterPoint, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointEnableMotor", UtcDaliDynamicsJointEnableMotor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointSetMotorVelocity", UtcDaliDynamicsJointSetMotorVelocity, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointSetMotorForce", UtcDaliDynamicsJointSetMotorForce, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsJointGetActor", UtcDaliDynamicsJointGetActor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsShapeConstructor", UtcDaliDynamicsShapeConstructor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsShapeNewCapsule", UtcDaliDynamicsShapeNewCapsule, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsShapeNewCone", UtcDaliDynamicsShapeNewCone, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsShapeNewCube", UtcDaliDynamicsShapeNewCube, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsShapeNewCylinder", UtcDaliDynamicsShapeNewCylinder, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsShapeNewMesh", UtcDaliDynamicsShapeNewMesh, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsShapeNewSphere", UtcDaliDynamicsShapeNewSphere, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsShapeGetType", UtcDaliDynamicsShapeGetType, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliStageInitializeDynamics", UtcDaliStageInitializeDynamics, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliStageGetDynamicsWorld", UtcDaliStageGetDynamicsWorld, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliStageTerminateDynamics", UtcDaliStageTerminateDynamics, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsWorldConstructor", UtcDaliDynamicsWorldConstructor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsWorldGravity", UtcDaliDynamicsWorldGravity, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsWorldDebugDrawMode", UtcDaliDynamicsWorldDebugDrawMode, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsWorldRootActor", UtcDaliDynamicsWorldRootActor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsWorldConfigConstructor", UtcDaliDynamicsWorldConfigConstructor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsWorldConfigNew", UtcDaliDynamicsWorldConfigNew, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliDynamicsWorldConfigType", UtcDaliDynamicsWorldConfigType, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliEncodedBufferImageNew01", UtcDaliEncodedBufferImageNew01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliEncodedBufferImageNew02", UtcDaliEncodedBufferImageNew02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliEncodedBufferImageNew03", UtcDaliEncodedBufferImageNew03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliHitTestAlgorithmWithFunctor", UtcDaliHitTestAlgorithmWithFunctor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliHitTestAlgorithmWithFunctorOnRenderTask", UtcDaliHitTestAlgorithmWithFunctorOnRenderTask, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliHitTestAlgorithmOrtho01", UtcDaliHitTestAlgorithmOrtho01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliHitTestAlgorithmOrtho02", UtcDaliHitTestAlgorithmOrtho02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliHitTestAlgorithmStencil", UtcDaliHitTestAlgorithmStencil, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorConstructorVoid", UtcDaliLightActorConstructorVoid, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorDestructor", UtcDaliLightActorDestructor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorNew", UtcDaliLightActorNew, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorDownCast", UtcDaliLightActorDownCast, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorDownCast2", UtcDaliLightActorDownCast2, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorSetGetLight", UtcDaliLightActorSetGetLight, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorSetGetActive", UtcDaliLightActorSetGetActive, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorMeshTest", UtcDaliLightActorMeshTest, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorDefaultProperties", UtcDaliLightActorDefaultProperties, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLightActorPropertyIndices", UtcDaliLightActorPropertyIndices, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLocklessBufferWriteRead01", UtcDaliLocklessBufferWriteRead01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLocklessBufferMultipleWrites01", UtcDaliLocklessBufferMultipleWrites01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLocklessBufferGetSize01", UtcDaliLocklessBufferGetSize01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorNew02", UtcDaliMeshActorNew02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorCreateNoMeshData", UtcDaliMeshActorCreateNoMeshData, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorCreateSetData01", UtcDaliMeshActorCreateSetData01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorCreateSetData02", UtcDaliMeshActorCreateSetData02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorCreateSetData03", UtcDaliMeshActorCreateSetData03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorCreateSetData04", UtcDaliMeshActorCreateSetData04, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorDownCast", UtcDaliMeshActorDownCast, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorDownCast2", UtcDaliMeshActorDownCast2, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorSetMaterial01", UtcDaliMeshActorSetMaterial01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorSetMaterial01b", UtcDaliMeshActorSetMaterial01b, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorSetMaterial02", UtcDaliMeshActorSetMaterial02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorSetMaterial02b", UtcDaliMeshActorSetMaterial02b, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorSetMaterial03", UtcDaliMeshActorSetMaterial03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorSetMaterial03b", UtcDaliMeshActorSetMaterial03b, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorGetMaterial01", UtcDaliMeshActorGetMaterial01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorGetMaterial02", UtcDaliMeshActorGetMaterial02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorSetLighting01", UtcDaliMeshActorSetLighting01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorSetLighting02", UtcDaliMeshActorSetLighting02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend01", UtcDaliMeshActorBlend01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend02", UtcDaliMeshActorBlend02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend03", UtcDaliMeshActorBlend03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend04", UtcDaliMeshActorBlend04, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend05", UtcDaliMeshActorBlend05, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend06", UtcDaliMeshActorBlend06, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend07", UtcDaliMeshActorBlend07, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend08", UtcDaliMeshActorBlend08, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBlend09", UtcDaliMeshActorBlend09, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorBoneUpdate01", UtcDaliMeshActorBoneUpdate01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshActorIndices", UtcDaliMeshActorIndices, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataNew", UtcDaliMeshDataNew, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataSetData", UtcDaliMeshDataSetData, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataAddToBoundingVolume", UtcDaliMeshDataAddToBoundingVolume, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataBoundingBox", UtcDaliMeshDataBoundingBox, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataGetVertexCount", UtcDaliMeshDataGetVertexCount, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataGetVertices", UtcDaliMeshDataGetVertices, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataGetFaceCount", UtcDaliMeshDataGetFaceCount, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataGetFaces", UtcDaliMeshDataGetFaces, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataTextureCoords", UtcDaliMeshDataTextureCoords, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataNormals", UtcDaliMeshDataNormals, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataGetMaterial", UtcDaliMeshDataGetMaterial, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataSetMaterial", UtcDaliMeshDataSetMaterial, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataGetBoneCount", UtcDaliMeshDataGetBoneCount, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMeshDataGetBones", UtcDaliMeshDataGetBones, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelConstructorVoid", UtcDaliModelConstructorVoid, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelNew", UtcDaliModelNew, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelDownCast", UtcDaliModelDownCast, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelGetLoadingState01", UtcDaliModelGetLoadingState01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelGetLoadingState02", UtcDaliModelGetLoadingState02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelGetLoadingState03", UtcDaliModelGetLoadingState03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelNumberOfAnimations", UtcDaliModelNumberOfAnimations, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelActorFactory", UtcDaliModelActorFactory, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelActorFactoryTwoMesh", UtcDaliModelActorFactoryTwoMesh, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelBuildAnimation01", UtcDaliModelBuildAnimation01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelBuildAnimation02", UtcDaliModelBuildAnimation02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelBuildAnimation03", UtcDaliModelBuildAnimation03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelBuildAnimation04", UtcDaliModelBuildAnimation04, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelBuildAnimation05", UtcDaliModelBuildAnimation05, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelBuildAnimation06", UtcDaliModelBuildAnimation06, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelBuildAnimation07", UtcDaliModelBuildAnimation07, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliModelBuildAnimation08", UtcDaliModelBuildAnimation08, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventConstructor", UtcDaliMouseWheelEventConstructor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventIsShiftModifier", UtcDaliMouseWheelEventIsShiftModifier, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventIsCtrlModifier", UtcDaliMouseWheelEventIsCtrlModifier, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventIsAltModifier", UtcDaliMouseWheelEventIsAltModifier, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventIsNotShiftModifier", UtcDaliMouseWheelEventIsNotShiftModifier, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventIsNotCtrlModifier", UtcDaliMouseWheelEventIsNotCtrlModifier, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventIsNotAltModifier", UtcDaliMouseWheelEventIsNotAltModifier, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventANDModifer", UtcDaliMouseWheelEventANDModifer, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventORModifer", UtcDaliMouseWheelEventORModifer, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliMouseWheelEventSignalling", UtcDaliMouseWheelEventSignalling, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistryGet", UtcDaliObjectRegistryGet, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalActorCreated", UtcDaliObjectRegistrySignalActorCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalCameraCreated", UtcDaliObjectRegistrySignalCameraCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalImageActorCreated", UtcDaliObjectRegistrySignalImageActorCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalLayerCreated", UtcDaliObjectRegistrySignalLayerCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalLightActorCreated", UtcDaliObjectRegistrySignalLightActorCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalMeshActorCreated", UtcDaliObjectRegistrySignalMeshActorCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalModelCreated", UtcDaliObjectRegistrySignalModelCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalTextActorCreated", UtcDaliObjectRegistrySignalTextActorCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalAnimationCreated", UtcDaliObjectRegistrySignalAnimationCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliObjectRegistrySignalShaderEffectCreated", UtcDaliObjectRegistrySignalShaderEffectCreated, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliRandomRangeMethod", UtcDaliRandomRangeMethod, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliRandomAxisMethod", UtcDaliRandomAxisMethod, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliRandomChanceMethod", UtcDaliRandomChanceMethod, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliRenderTaskSetScreenToFrameBufferMappingActor", UtcDaliRenderTaskSetScreenToFrameBufferMappingActor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliShaderEffectFromProperties01", UtcDaliShaderEffectFromProperties01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliShaderEffectFromProperties02", UtcDaliShaderEffectFromProperties02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliShaderEffectFromProperties03", UtcDaliShaderEffectFromProperties03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingGetColorMode", UtcDaliScriptingGetColorMode, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingGetPositionInheritanceMode", UtcDaliScriptingGetPositionInheritanceMode, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingGetDrawMode", UtcDaliScriptingGetDrawMode, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingGetAnchorConstant", UtcDaliScriptingGetAnchorConstant, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingNewImageNegative", UtcDaliScriptingNewImageNegative, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingNewImage", UtcDaliScriptingNewImage, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingNewShaderEffect", UtcDaliScriptingNewShaderEffect, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingNewActorNegative", UtcDaliScriptingNewActorNegative, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingNewActorProperties", UtcDaliScriptingNewActorProperties, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingNewActorChildren", UtcDaliScriptingNewActorChildren, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingCreatePropertyMapActor", UtcDaliScriptingCreatePropertyMapActor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingCreatePropertyMapImage", UtcDaliScriptingCreatePropertyMapImage, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliScriptingGetEnumerationTemplates", UtcDaliScriptingGetEnumerationTemplates, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetYFromMonotonicX", UtcDaliSplineGetYFromMonotonicX, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetKnot01", utcDaliSplineGetKnot01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetKnot02", utcDaliSplineGetKnot02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetKnot03", utcDaliSplineGetKnot03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetInTangent01", utcDaliSplineGetInTangent01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetInTangent02", utcDaliSplineGetInTangent02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetInTangent03", utcDaliSplineGetInTangent03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetOutTangent01", utcDaliSplineGetOutTangent01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetOutTangent02", utcDaliSplineGetOutTangent02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGetOutTangent03", utcDaliSplineGetOutTangent03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGenerateControlPoints01", utcDaliSplineGenerateControlPoints01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGenerateControlPoints02", utcDaliSplineGenerateControlPoints02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"utcDaliSplineGenerateControlPoints03", utcDaliSplineGenerateControlPoints03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetY01", UtcDaliSplineGetY01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetY02", UtcDaliSplineGetY02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetY02b", UtcDaliSplineGetY02b, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetY03", UtcDaliSplineGetY03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetY04", UtcDaliSplineGetY04, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetY04b", UtcDaliSplineGetY04b, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetPoint01", UtcDaliSplineGetPoint01, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetPoint02", UtcDaliSplineGetPoint02, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetPoint03", UtcDaliSplineGetPoint03, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetPoint04", UtcDaliSplineGetPoint04, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetPoint05", UtcDaliSplineGetPoint05, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliSplineGetPoint06", UtcDaliSplineGetPoint06, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryCreateDaliObjects", UtcDaliTypeRegistryCreateDaliObjects, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryActionViaBaseHandle", UtcDaliTypeRegistryActionViaBaseHandle, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryNames", UtcDaliTypeRegistryNames, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryNameEquivalence", UtcDaliTypeRegistryNameEquivalence, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryCustomActor", UtcDaliTypeRegistryCustomActor, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryCustomSignalFailure", UtcDaliTypeRegistryCustomSignalFailure, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryInitFunctions", UtcDaliTypeRegistryInitFunctions, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryNameInitFunctions", UtcDaliTypeRegistryNameInitFunctions, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPropertyRegistration", UtcDaliPropertyRegistration, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPropertyRegistrationIndexOutOfBounds", UtcDaliPropertyRegistrationIndexOutOfBounds, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPropertyRegistrationFunctions", UtcDaliPropertyRegistrationFunctions, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPropertyRegistrationAddSameIndex", UtcDaliPropertyRegistrationAddSameIndex, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPropertyRegistrationPropertyWritable", UtcDaliPropertyRegistrationPropertyWritable, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPropertyRegistrationPropertyAnimatable", UtcDaliPropertyRegistrationPropertyAnimatable, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPropertyRegistrationInvalidGetAndSet", UtcDaliPropertyRegistrationInvalidGetAndSet, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliLongPressGestureDetectorTypeRegistry", UtcDaliLongPressGestureDetectorTypeRegistry, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPanGestureDetectorTypeRegistry", UtcDaliPanGestureDetectorTypeRegistry, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliPinchGestureDetectorTypeRegistry", UtcDaliPinchGestureDetectorTypeRegistry, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTapGestureDetectorTypeRegistry", UtcDaliTapGestureDetectorTypeRegistry, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliTypeRegistryNamedType", UtcDaliTypeRegistryNamedType, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliUtf8SequenceLength", UtcDaliUtf8SequenceLength, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliEmptyVectorInt", UtcDaliEmptyVectorInt, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorInt", UtcDaliVectorInt, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorIntCopy", UtcDaliVectorIntCopy, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorIntResize", UtcDaliVectorIntResize, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorIntErase", UtcDaliVectorIntErase, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorDoubleRemove", UtcDaliVectorDoubleRemove, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorIntSwap", UtcDaliVectorIntSwap, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorIterate", UtcDaliVectorIterate, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorPair", UtcDaliVectorPair, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorAsserts", UtcDaliVectorAsserts, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorAcidTest", UtcDaliVectorAcidTest, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {"UtcDaliVectorComplex", UtcDaliVectorComplex, utc_dali_unmanaged_startup, utc_dali_unmanaged_cleanup},
    {NULL, NULL}
};

#endif // __TCT_DALI_UNMANAGED_CORE_H__
