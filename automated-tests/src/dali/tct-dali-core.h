#ifndef __TCT_DALI_CORE_H__
#define __TCT_DALI_CORE_H__

#include "testcase.h"

extern void utc_dali_active_constraint_startup(void);
extern void utc_dali_active_constraint_cleanup(void);
extern void utc_dali_actor_startup(void);
extern void utc_dali_actor_cleanup(void);
extern void utc_dali_alpha_functions_startup(void);
extern void utc_dali_alpha_functions_cleanup(void);
extern void utc_dali_angle_axis_startup(void);
extern void utc_dali_angle_axis_cleanup(void);
extern void utc_dali_animatable_mesh_startup(void);
extern void utc_dali_animatable_mesh_cleanup(void);
extern void utc_dali_animation_startup(void);
extern void utc_dali_animation_cleanup(void);
extern void utc_dali_any_startup(void);
extern void utc_dali_any_cleanup(void);
extern void utc_base_handle_startup(void);
extern void utc_base_handle_cleanup(void);
extern void utc_dali_bitmap_image_startup(void);
extern void utc_dali_bitmap_image_cleanup(void);
extern void camera_actor_test_startup(void);
extern void camera_actor_test_cleanup(void);
extern void utc_dali_character_startup(void);
extern void utc_dali_character_cleanup(void);
extern void utc_dali_constraint_startup(void);
extern void utc_dali_constraint_cleanup(void);
extern void custom_actor_test_startup(void);
extern void custom_actor_test_cleanup(void);
extern void utc_dali_degree_startup(void);
extern void utc_dali_degree_cleanup(void);
extern void utc_dali_event_processing_startup(void);
extern void utc_dali_event_processing_cleanup(void);
extern void utc_dali_font_startup(void);
extern void utc_dali_font_cleanup(void);
extern void utc_dali_font_parameters_startup(void);
extern void utc_dali_font_parameters_cleanup(void);
extern void utc_dali_framebuffer_startup(void);
extern void utc_dali_framebuffer_cleanup(void);
extern void utc_dali_gesture_startup(void);
extern void utc_dali_gesture_cleanup(void);
extern void utc_dali_gesture_detector_startup(void);
extern void utc_dali_gesture_detector_cleanup(void);
extern void handle_test_startup(void);
extern void handle_test_cleanup(void);
extern void utc_dali_image_startup(void);
extern void utc_dali_image_cleanup(void);
extern void image_actor_test_startup(void);
extern void image_actor_test_cleanup(void);
extern void utc_dali_image_attributes_startup(void);
extern void utc_dali_image_attributes_cleanup(void);
extern void utc_dali_key_event_startup(void);
extern void utc_dali_key_event_cleanup(void);
extern void layer_test_startup(void);
extern void layer_test_cleanup(void);
extern void utc_dali_long_press_gesture_startup(void);
extern void utc_dali_long_press_gesture_cleanup(void);
extern void utc_dali_long_press_gesture_detector_startup(void);
extern void utc_dali_long_press_gesture_detector_cleanup(void);
extern void utc_dali_material_startup(void);
extern void utc_dali_material_cleanup(void);
extern void utc_dali_math_utils_startup(void);
extern void utc_dali_math_utils_cleanup(void);
extern void utc_dali_matrix_startup(void);
extern void utc_dali_matrix_cleanup(void);
extern void utc_dali_matrix3_startup(void);
extern void utc_dali_matrix3_cleanup(void);
extern void mesh_actor_test_startup(void);
extern void mesh_actor_test_cleanup(void);
extern void utc_dali_pan_gesture_startup(void);
extern void utc_dali_pan_gesture_cleanup(void);
extern void utc_dali_pan_gesture_detector_startup(void);
extern void utc_dali_pan_gesture_detector_cleanup(void);
extern void utc_dali_pinch_gesture_startup(void);
extern void utc_dali_pinch_gesture_cleanup(void);
extern void utc_dali_pinch_gesture_detector_startup(void);
extern void utc_dali_pinch_gesture_detector_cleanup(void);
extern void utc_dali_pixel_startup(void);
extern void utc_dali_pixel_cleanup(void);
extern void utc_dali_property_notification_startup(void);
extern void utc_dali_property_notification_cleanup(void);
extern void utc_dali_quaternion_startup(void);
extern void utc_dali_quaternion_cleanup(void);
extern void utc_dali_radian_startup(void);
extern void utc_dali_radian_cleanup(void);
extern void utc_dali_rect_startup(void);
extern void utc_dali_rect_cleanup(void);
extern void utc_dali_render_task_startup(void);
extern void utc_dali_render_task_cleanup(void);
extern void utc_dali_render_task_list_startup(void);
extern void utc_dali_render_task_list_cleanup(void);
extern void renderable_actor_startup(void);
extern void renderable_actor_cleanup(void);
extern void utc_dali_shader_effect_startup(void);
extern void utc_dali_shader_effect_cleanup(void);
extern void utc_dali_signal_templates_functors_startup(void);
extern void utc_dali_signal_templates_functors_cleanup(void);
extern void utc_dali_signal_templates_startup(void);
extern void utc_dali_signal_templates_cleanup(void);
extern void stage_test_startup(void);
extern void stage_test_cleanup(void);
extern void utc_dali_tap_gesture_startup(void);
extern void utc_dali_tap_gesture_cleanup(void);
extern void utc_dali_tap_gesture_detector_startup(void);
extern void utc_dali_tap_gesture_detector_cleanup(void);
extern void utc_dali_text_startup(void);
extern void utc_dali_text_cleanup(void);
extern void text_actor_test_startup(void);
extern void text_actor_test_cleanup(void);
extern void utc_dali_text_style_startup(void);
extern void utc_dali_text_style_cleanup(void);
extern void utc_dali_touch_event_combiner_startup(void);
extern void utc_dali_touch_event_combiner_cleanup(void);
extern void utc_dali_touch_processing_startup(void);
extern void utc_dali_touch_processing_cleanup(void);
extern void utc_dali_vector2_startup(void);
extern void utc_dali_vector2_cleanup(void);
extern void utc_dali_vector3_startup(void);
extern void utc_dali_vector3_cleanup(void);
extern void utc_dali_vector4_startup(void);
extern void utc_dali_vector4_cleanup(void);

extern int UtcDaliConstraintGetTargetObject(void);
extern int UtcDaliConstraintGetTargetProperty(void);
extern int UtcDaliConstraintSetWeight(void);
extern int UtcDaliConstraintGetCurrentWeight(void);
extern int UtcDaliConstraintSignalApplied(void);
extern int UtcDaliConstraintRemove(void);
extern int UtcDaliConstraintCallback(void);
extern int UtcDaliConstraintProperties(void);
extern int UtcDaliActorNew(void);
extern int UtcDaliActorDownCast(void);
extern int UtcDaliActorDownCast2(void);
extern int UtcDaliActorGetName(void);
extern int UtcDaliActorSetName(void);
extern int UtcDaliActorGetId(void);
extern int UtcDaliActorIsRoot(void);
extern int UtcDaliActorOnStage(void);
extern int UtcDaliActorIsLayer(void);
extern int UtcDaliActorGetLayer(void);
extern int UtcDaliActorAdd(void);
extern int UtcDaliActorRemove01(void);
extern int UtcDaliActorRemove02(void);
extern int UtcDaliActorGetChildCount(void);
extern int UtcDaliActorGetChildren01(void);
extern int UtcDaliActorGetChildren02(void);
extern int UtcDaliActorGetParent01(void);
extern int UtcDaliActorGetParent02(void);
extern int UtcDaliActorSetParentOrigin(void);
extern int UtcDaliActorGetCurrentParentOrigin(void);
extern int UtcDaliActorSetAnchorPoint(void);
extern int UtcDaliActorGetCurrentAnchorPoint(void);
extern int UtcDaliActorSetSize01(void);
extern int UtcDaliActorSetSize02(void);
extern int UtcDaliActorSetSize03(void);
extern int UtcDaliActorSetSize04(void);
extern int UtcDaliActorGetCurrentSize(void);
extern int UtcDaliActorSetPosition01(void);
extern int UtcDaliActorSetPosition02(void);
extern int UtcDaliActorSetPosition03(void);
extern int UtcDaliActorSetX(void);
extern int UtcDaliActorSetY(void);
extern int UtcDaliActorSetZ(void);
extern int UtcDaliActorMoveBy(void);
extern int UtcDaliActorGetCurrentPosition(void);
extern int UtcDaliActorGetCurrentWorldPosition(void);
extern int UtcDaliActorInheritPosition(void);
extern int UtcDaliActorSetRotation01(void);
extern int UtcDaliActorSetRotation02(void);
extern int UtcDaliActorRotateBy01(void);
extern int UtcDaliActorRotateBy02(void);
extern int UtcDaliActorGetCurrentRotation(void);
extern int UtcDaliActorGetCurrentWorldRotation(void);
extern int UtcDaliActorSetScale01(void);
extern int UtcDaliActorSetScale02(void);
extern int UtcDaliActorSetScale03(void);
extern int UtcDaliActorScaleBy(void);
extern int UtcDaliActorGetCurrentScale(void);
extern int UtcDaliActorGetCurrentWorldScale(void);
extern int UtcDaliActorInheritScale(void);
extern int UtcDaliActorSetVisible(void);
extern int UtcDaliActorIsVisible(void);
extern int UtcDaliActorSetOpacity(void);
extern int UtcDaliActorGetCurrentOpacity(void);
extern int UtcDaliActorSetSensitive(void);
extern int UtcDaliActorIsSensitive(void);
extern int UtcDaliActorSetInheritShaderEffect(void);
extern int UtcDaliActorGetInheritShaderEffect(void);
extern int UtcDaliActorSetShaderEffect(void);
extern int UtcDaliActorGetShaderEffect(void);
extern int UtcDaliActorRemoveShaderEffect01(void);
extern int UtcDaliActorRemoveShaderEffect02(void);
extern int UtcDaliActorSetColor(void);
extern int UtcDaliActorGetCurrentColor(void);
extern int UtcDaliActorGetCurrentWorldColor(void);
extern int UtcDaliActorSetColorMode(void);
extern int UtcDaliActorScreenToLocal(void);
extern int UtcDaliActorSetLeaveRequired(void);
extern int UtcDaliActorGetLeaveRequired(void);
extern int UtcDaliActorSetKeyboardFocusable(void);
extern int UtcDaliActorIsKeyboardFocusable(void);
extern int UtcDaliActorApplyConstraint(void);
extern int UtcDaliActorApplyConstraintAppliedCallback(void);
extern int UtcDaliActorRemoveConstraints(void);
extern int UtcDaliActorRemoveConstraint(void);
extern int UtcDaliActorTouchedSignal(void);
extern int UtcDaliActorSetSizeSignal(void);
extern int UtcDaliActorOnOffStageSignal(void);
extern int UtcDaliActorFindChildByName(void);
extern int UtcDaliActorFindChildByAlias(void);
extern int UtcDaliActorFindChildById(void);
extern int UtcDaliActorHitTest(void);
extern int UtcDaliActorSetDrawMode(void);
extern int UtcDaliActorSetDrawModeOverlayRender(void);
extern int UtcDaliActorSetDrawModeOverlayHitTest(void);
extern int UtcDaliActorGetCurrentWorldMatrix(void);
extern int UtcDaliActorConstrainedToWorldMatrix(void);
extern int UtcDaliActorUnparent(void);
extern int UtcDaliActorGetChildAt(void);
extern int UtcDaliActorSetGetOverlay(void);
extern int UtcDaliActorDynamics(void);
extern int UtcDaliActorCreateDestroy(void);
extern int UtcDaliAlphaFunctionsDefault(void);
extern int UtcDaliAlphaFunctionsLinear(void);
extern int UtcDaliAlphaFunctionsReverse(void);
extern int UtcDaliAlphaFunctionsEaseIn(void);
extern int UtcDaliAlphaFunctionsEaseOut(void);
extern int UtcDaliAlphaFunctionsEaseInOut(void);
extern int UtcDaliAlphaFunctionsEaseInSine(void);
extern int UtcDaliAlphaFunctionsEaseOutSine(void);
extern int UtcDaliAlphaFunctionsEaseInOutSine(void);
extern int UtcDaliAlphaFunctionsBounce(void);
extern int UtcDaliAlphaFunctionsBounceBack(void);
extern int UtcDaliAlphaFunctionsEaseOutBack(void);
extern int UtcDaliAlphaFunctionsSin(void);
extern int UtcDaliAlphaFunctionsSin2x(void);
extern int UtcDaliAlphaFunctionsSquare(void);
extern int UtcDaliAlphaFunctionsEaseInSine33(void);
extern int UtcDaliAlphaFunctionsEaseOutSine33(void);
extern int UtcDaliAlphaFunctionsEaseInOutSineXX(void);
extern int UtcDaliAlphaFunctionsDoubleEaseInOutSine60(void);
extern int UtcDaliAlphaFunctionsEaseOutQuint50(void);
extern int UtcDaliAlphaFunctionsEaseOutQuint80(void);
extern int UtcDaliAlphaFunctionsEaseInBack(void);
extern int UtcDaliAlphaFunctionsEaseInOutBack(void);
extern int UtcDaliAngleAxisNew01(void);
extern int UtcDaliAngleAxisNew02(void);
extern int UtcDaliAngleAxisNew03(void);
extern int UtcDaliAngleAxisAssign(void);
extern int UtcDaliAngleAxisCopy(void);
extern int UtcDaliAnimatableMeshConstructor01(void);
extern int UtcDaliAnimatableMeshConstructor02(void);
extern int UtcDaliAnimatableMeshConstructor03(void);
extern int UtcDaliAnimatableMeshNew01(void);
extern int UtcDaliAnimatableMeshNew02(void);
extern int UtcDaliAnimatableMeshNew03(void);
extern int UtcDaliAnimatableMeshNew04(void);
extern int UtcDaliAnimatableMeshNew05(void);
extern int UtcDaliAnimatableMeshNew06(void);
extern int UtcDaliAnimatableMeshDownCast01(void);
extern int UtcDaliAnimatableMeshGetPropertyIndex01(void);
extern int UtcDaliAnimatableMeshGetPropertyIndex02(void);
extern int UtcDaliAnimatableMeshGetPropertyIndex03(void);
extern int UtcDaliAnimatableMeshGetPropertyIndex04(void);
extern int UtcDaliAnimatableMeshOperatorArray01(void);
extern int UtcDaliAnimatableMeshOperatorArray02(void);
extern int UtcDaliAnimatableMeshAnimateVertex01(void);
extern int UtcDaliAnimatableVertexSettersAndGetters(void);
extern int UtcDaliAnimatableMeshProperties(void);
extern int UtcDaliAnimatableMeshExceedVertices(void);
extern int UtcDaliAnimationNew01(void);
extern int UtcDaliAnimationNew02(void);
extern int UtcDaliAnimationDownCast(void);
extern int UtcDaliAnimationSetDuration(void);
extern int UtcDaliAnimationGetDuration(void);
extern int UtcDaliAnimationSetLooping(void);
extern int UtcDaliAnimationIsLooping(void);
extern int UtcDaliAnimationSetEndAction(void);
extern int UtcDaliAnimationGetEndAction(void);
extern int UtcDaliAnimationGetDestroyAction(void);
extern int UtcDaliAnimationSetDefaultAlphaFunction(void);
extern int UtcDaliAnimationGetDefaultAlphaFunction(void);
extern int UtcDaliAnimationPlay(void);
extern int UtcDaliAnimationPlayOffStage(void);
extern int UtcDaliAnimationPlayDiscardHandle(void);
extern int UtcDaliAnimationPlayStopDiscardHandle(void);
extern int UtcDaliAnimationPause(void);
extern int UtcDaliAnimationStop(void);
extern int UtcDaliAnimationStopSetPosition(void);
extern int UtcDaliAnimationClear(void);
extern int UtcDaliAnimationSignalFinish(void);
extern int UtcDaliAnimationAnimateByBoolean(void);
extern int UtcDaliAnimationAnimateByBooleanAlphaFunction(void);
extern int UtcDaliAnimationAnimateByBooleanTimePeriod(void);
extern int UtcDaliAnimationAnimateByBooleanAlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateByFloat(void);
extern int UtcDaliAnimationAnimateByFloatAlphaFunction(void);
extern int UtcDaliAnimationAnimateByFloatTimePeriod(void);
extern int UtcDaliAnimationAnimateByFloatAlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateByVector2(void);
extern int UtcDaliAnimationAnimateByVector2AlphaFunction(void);
extern int UtcDaliAnimationAnimateByVector2TimePeriod(void);
extern int UtcDaliAnimationAnimateByVector2AlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateByVector3(void);
extern int UtcDaliAnimationAnimateByVector3AlphaFunction(void);
extern int UtcDaliAnimationAnimateByVector3TimePeriod(void);
extern int UtcDaliAnimationAnimateByVector3AlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateByVector4(void);
extern int UtcDaliAnimationAnimateByVector4AlphaFunction(void);
extern int UtcDaliAnimationAnimateByVector4TimePeriod(void);
extern int UtcDaliAnimationAnimateByVector4AlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateByActorPosition(void);
extern int UtcDaliAnimationAnimateByActorPositionAlphaFunction(void);
extern int UtcDaliAnimationAnimateByActorPositionTimePeriod(void);
extern int UtcDaliAnimationAnimateByActorPositionAlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateToBoolean(void);
extern int UtcDaliAnimationAnimateToBooleanAlphaFunction(void);
extern int UtcDaliAnimationAnimateToBooleanTimePeriod(void);
extern int UtcDaliAnimationAnimateToBooleanAlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateToFloat(void);
extern int UtcDaliAnimationAnimateToFloatAlphaFunction(void);
extern int UtcDaliAnimationAnimateToFloatTimePeriod(void);
extern int UtcDaliAnimationAnimateToFloatAlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateToVector2(void);
extern int UtcDaliAnimationAnimateToVector2AlphaFunction(void);
extern int UtcDaliAnimationAnimateToVector2TimePeriod(void);
extern int UtcDaliAnimationAnimateToVector2AlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateToVector3(void);
extern int UtcDaliAnimationAnimateToVector3AlphaFunction(void);
extern int UtcDaliAnimationAnimateToVector3TimePeriod(void);
extern int UtcDaliAnimationAnimateToVector3AlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateToVector3Component(void);
extern int UtcDaliAnimationAnimateToVector4(void);
extern int UtcDaliAnimationAnimateToVector4AlphaFunction(void);
extern int UtcDaliAnimationAnimateToVector4TimePeriod(void);
extern int UtcDaliAnimationAnimateToVector4AlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateToActorParentOrigin(void);
extern int UtcDaliAnimationAnimateToActorParentOriginX(void);
extern int UtcDaliAnimationAnimateToActorParentOriginY(void);
extern int UtcDaliAnimationAnimateToActorParentOriginZ(void);
extern int UtcDaliAnimationAnimateToActorAnchorPoint(void);
extern int UtcDaliAnimationAnimateToActorAnchorPointX(void);
extern int UtcDaliAnimationAnimateToActorAnchorPointY(void);
extern int UtcDaliAnimationAnimateToActorAnchorPointZ(void);
extern int UtcDaliAnimationAnimateToActorSize(void);
extern int UtcDaliAnimationAnimateToActorSizeWidth(void);
extern int UtcDaliAnimationAnimateToActorSizeHeight(void);
extern int UtcDaliAnimationAnimateToActorSizeDepth(void);
extern int UtcDaliAnimationAnimateToActorPosition(void);
extern int UtcDaliAnimationAnimateToActorPositionX(void);
extern int UtcDaliAnimationAnimateToActorPositionY(void);
extern int UtcDaliAnimationAnimateToActorPositionZ(void);
extern int UtcDaliAnimationAnimateToActorPositionAlphaFunction(void);
extern int UtcDaliAnimationAnimateToActorPositionTimePeriod(void);
extern int UtcDaliAnimationAnimateToActorPositionAlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateToActorRotationAngleAxis(void);
extern int UtcDaliAnimationAnimateToActorRotationQuaternion(void);
extern int UtcDaliAnimationAnimateToActorRotationAlphaFunction(void);
extern int UtcDaliAnimationAnimateToActorRotationTimePeriod(void);
extern int UtcDaliAnimationAnimateToActorRotationAlphaFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateToActorScale(void);
extern int UtcDaliAnimationAnimateToActorScaleX(void);
extern int UtcDaliAnimationAnimateToActorScaleY(void);
extern int UtcDaliAnimationAnimateToActorScaleZ(void);
extern int UtcDaliAnimationAnimateToActorColor(void);
extern int UtcDaliAnimationAnimateToActorColorRed(void);
extern int UtcDaliAnimationAnimateToActorColorGreen(void);
extern int UtcDaliAnimationAnimateToActorColorBlue(void);
extern int UtcDaliAnimationAnimateToActorColorAlpha(void);
extern int UtcDaliAnimationKeyFrames01(void);
extern int UtcDaliAnimationKeyFrames02(void);
extern int UtcDaliAnimationKeyFrames03(void);
extern int UtcDaliAnimationKeyFrames04(void);
extern int UtcDaliAnimationKeyFrames05(void);
extern int UtcDaliAnimationKeyFrames06(void);
extern int UtcDaliAnimationAnimateBetweenActorColorAlpha(void);
extern int UtcDaliAnimationAnimateBetweenActorColor(void);
extern int UtcDaliAnimationAnimateBetweenActorVisible01(void);
extern int UtcDaliAnimationAnimateBetweenActorRotation02(void);
extern int UtcDaliAnimationMoveByFloat3(void);
extern int UtcDaliAnimationMoveByVector3Alpha(void);
extern int UtcDaliAnimationMoveByVector3AlphaFloat2(void);
extern int UtcDaliAnimationMoveToFloat3(void);
extern int UtcDaliAnimationMoveToVector3Alpha(void);
extern int UtcDaliAnimationMoveToVector3AlphaFloat2(void);
extern int UtcDaliAnimationMove(void);
extern int UtcDaliAnimationRotateByDegreeVector3(void);
extern int UtcDaliAnimationRotateByRadianVector3(void);
extern int UtcDaliAnimationRotateByDegreeVector3Alpha(void);
extern int UtcDaliAnimationRotateByRadianVector3Alpha(void);
extern int UtcDaliAnimationRotateByDegreeVector3AlphaFloat2(void);
extern int UtcDaliAnimationRotateByRadianVector3AlphaFloat2(void);
extern int UtcDaliAnimationRotateToDegreeVector3(void);
extern int UtcDaliAnimationRotateToRadianVector3(void);
extern int UtcDaliAnimationRotateToQuaternion(void);
extern int UtcDaliAnimationRotateToDegreeVector3Alpha(void);
extern int UtcDaliAnimationRotateToRadianVector3Alpha(void);
extern int UtcDaliAnimationRotateToQuaternionAlpha(void);
extern int UtcDaliAnimationRotateToDegreeVector3AlphaFloat2(void);
extern int UtcDaliAnimationRotateToRadianVector3AlphaFloat2(void);
extern int UtcDaliAnimationRotateToQuaternionAlphaFloat2(void);
extern int UtcDaliAnimationRotate(void);
extern int UtcDaliAnimationScaleBy(void);
extern int UtcDaliAnimationScaleTo(void);
extern int UtcDaliAnimationShow(void);
extern int UtcDaliAnimationHide(void);
extern int UtcDaliAnimationShowHideAtEnd(void);
extern int UtcDaliAnimationOpacityBy(void);
extern int UtcDaliAnimationOpacityTo(void);
extern int UtcDaliAnimationColorBy(void);
extern int UtcDaliAnimationColorTo(void);
extern int UtcDaliAnimationResize(void);
extern int UtcDaliAnimationAnimateBool(void);
extern int UtcDaliAnimationAnimateFloat(void);
extern int UtcDaliAnimationAnimateVector2(void);
extern int UtcDaliAnimationAnimateVector3(void);
extern int UtcDaliAnimationAnimateVector4(void);
extern int UtcDaliAnimationAnimateQuaternion(void);
extern int UtcDaliKeyFramesCreateDestroy(void);
extern int UtcDaliKeyFramesDownCast(void);
extern int UtcDaliAnimationResizeByXY(void);
extern int UtcDaliAnimationAnimateBetweenActorColorTimePeriod(void);
extern int UtcDaliAnimationAnimateBetweenActorColorFunction(void);
extern int UtcDaliAnimationAnimateBetweenActorColorFunctionTimePeriod(void);
extern int UtcDaliAnimationAnimateVector3Func(void);
extern int UtcDaliAnimationCreateDestroy(void);
extern int UtcDaliAnyConstructors(void);
extern int UtcDaliAnyAssignmentOperators(void);
extern int UtcDaliAnyNegativeAssignmentOperators(void);
extern int UtcDaliAnyGetType(void);
extern int UtcDaliAnyGet(void);
extern int UtcDaliAnyNegativeGet(void);
extern int UtcDaliBaseHandleConstructorVoid(void);
extern int UtcDaliBaseHandleCopyConstructor(void);
extern int UtcDaliBaseHandleAssignmentOperator(void);
extern int UtcDaliBaseHandleGetBaseObject(void);
extern int UtcDaliBaseHandleReset(void);
extern int UtcDaliBaseHandleEqualityOperator01(void);
extern int UtcDaliBaseHandleEqualityOperator02(void);
extern int UtcDaliBaseHandleInequalityOperator01(void);
extern int UtcDaliBaseHandleInequalityOperator02(void);
extern int UtcDaliBaseHandleStlVector(void);
extern int UtcDaliBaseHandleDoAction(void);
extern int UtcDaliBaseHandleConnectSignal(void);
extern int UtcDaliBaseHandleGetTypeName(void);
extern int UtcDaliBaseHandleGetObjectPtr(void);
extern int UtcDaliBaseHandleBooleanCast(void);
extern int UtcDaliBitmapImageNew01(void);
extern int UtcDaliBitmapImageNew02(void);
extern int UtcDaliBitmapImageNewWithPolicy01(void);
extern int UtcDaliBitmapImageNewWithPolicy02(void);
extern int UtcDaliBitmapImageDownCast(void);
extern int UtcDaliBitmapImageDownCast2(void);
extern int UtcDaliBitmapImageWHITE(void);
extern int UtcDaliBitmapImageGetBuffer(void);
extern int UtcDaliBitmapImageGetBufferSize(void);
extern int UtcDaliBitmapImageGetBufferStride(void);
extern int UtcDaliBitmapImageIsDataExternal(void);
extern int UtcDaliBitmapImageUpdate01(void);
extern int UtcDaliBitmapImageUpdate02(void);
extern int UtcDaliBitmapImageUploadedSignal01(void);
extern int UtcDaliBitmapImageUploadedSignal02(void);
extern int UtcDaliCameraActorConstructorVoid(void);
extern int UtcDaliCameraActorNew(void);
extern int UtcDaliCameraActorDownCast(void);
extern int UtcDaliCameraActorDownCast2(void);
extern int UtcDaliCameraActorSetCameraOffStage(void);
extern int UtcDaliCameraActorSetCameraOnStage(void);
extern int UtcDaliCameraActorGetCamera(void);
extern int UtcDaliCameraActorDefaultProperties(void);
extern int UtcDaliCameraActorSetTarget(void);
extern int UtcDaliCameraActorSetType01(void);
extern int UtcDaliCameraActorSetType02(void);
extern int UtcDaliCameraActorSetFieldOfView(void);
extern int UtcDaliCameraActorSetAspectRatio(void);
extern int UtcDaliCameraActorSetNearClippingPlane(void);
extern int UtcDaliCameraActorSetFarClippingPlane(void);
extern int UtcDaliCameraActorSetTargetPosition(void);
extern int UtcDaliCameraActorSetInvertYAxis(void);
extern int UtcDaliCameraActorModelView(void);
extern int UtcDaliCameraActorSetPerspectiveProjection(void);
extern int UtcDaliCameraActorSetOrthographicProjection01(void);
extern int UtcDaliCameraActorSetOrthographicProjection02(void);
extern int UtcDaliCameraActorSetOrthographicProjection03(void);
extern int UtcDaliCameraActorReadProjectionMatrix(void);
extern int UtcDaliCameraActorAnimatedProperties(void);
extern int UtcDaliCameraActorPropertyIndices(void);
extern int UtcDaliCameraActorCheckLookAtAndFreeLookViews01(void);
extern int UtcDaliCameraActorCheckLookAtAndFreeLookViews02(void);
extern int UtcDaliCameraActorCheckLookAtAndFreeLookViews03(void);
extern int UtcDaliCharacterCopyConstructor(void);
extern int UtcDaliCharacterComparissonOperators(void);
extern int UtcDaliCharacterAssignmentOperator(void);
extern int UtcDaliCharacterIsLeftToRight(void);
extern int UtcDaliGetCharacterDirection(void);
extern int UtcDaliCharacterIsWhiteSpace(void);
extern int UtcDaliCharacterIsNewLine(void);
extern int UtcDaliConstraintNewBoolean(void);
extern int UtcDaliConstraintNewFloat(void);
extern int UtcDaliConstraintNewVector2(void);
extern int UtcDaliConstraintNewVector3(void);
extern int UtcDaliConstraintNewVector4(void);
extern int UtcDaliConstraintNewMatrix(void);
extern int UtcDaliConstraintNewMatrix3(void);
extern int UtcDaliConstraintNewQuaternion(void);
extern int UtcDaliConstraintNewOffStageBoolean(void);
extern int UtcDaliConstraintNewOffStageFloat(void);
extern int UtcDaliConstraintNewOffStageVector2(void);
extern int UtcDaliConstraintNewOffStageVector3(void);
extern int UtcDaliConstraintNewOffStageVector4(void);
extern int UtcDaliConstraintNewOffStageQuaternion(void);
extern int UtcDaliConstraintNewLocalInput(void);
extern int UtcDaliConstraintNewParentInput(void);
extern int UtcDaliConstraintNewInput1(void);
extern int UtcDaliConstraintNewInput2(void);
extern int UtcDaliConstraintNewInput3(void);
extern int UtcDaliConstraintNewInput4(void);
extern int UtcDaliConstraintNewInput5(void);
extern int UtcDaliConstraintNewInput6(void);
extern int UtcDaliConstraintDownCast(void);
extern int UtcDaliConstraintSetApplyTime(void);
extern int UtcDaliConstraintGetApplyTime(void);
extern int UtcDaliConstraintSetRemoveTime(void);
extern int UtcDaliConstraintGetRemoveTime(void);
extern int UtcDaliConstraintSetAlphaFunction(void);
extern int UtcDaliConstraintGetAlphaFunction(void);
extern int UtcDaliConstraintSetRemoveAction(void);
extern int UtcDaliConstraintGetRemoveAction(void);
extern int UtcDaliConstraintRemoveDuringApply(void);
extern int UtcDaliConstraintImmediateRemoveDuringApply(void);
extern int UtcDaliConstraintActorSize(void);
extern int UtcDaliConstraintActorSizeWidth(void);
extern int UtcDaliConstraintActorSizeHeight(void);
extern int UtcDaliConstraintActorSizeDepth(void);
extern int UtcDaliConstraintInputWorldPosition(void);
extern int UtcDaliConstraintInputWorldRotation(void);
extern int UtcDaliConstraintInputWorldScale(void);
extern int UtcDaliConstraintInputWorldColor(void);
extern int UtcDaliConstraintInvalidInputProperty(void);
extern int UtcDaliBuiltinConstraintParentSize(void);
extern int UtcDaliBuiltinConstraintParentSizeRelative(void);
extern int UtcDaliBuiltinConstraintScaleToFitConstraint(void);
extern int UtcDaliBuiltinConstraintScaleToFitKeepAspectRatio(void);
extern int UtcDaliBuiltinConstraintScaleToFillKeepAspectRatio(void);
extern int UtcDaliBuiltinConstraintScaleToFillXYKeepAspectRatio(void);
extern int UtcDaliBuiltinConstraintShrinkInsideKeepAspectRatioConstraint(void);
extern int UtcDaliBuiltinConstraintMultiplyConstraint(void);
extern int UtcDaliBuiltinConstraintDivideConstraint(void);
extern int UtcDaliBuiltinConstraintEqualToConstraint(void);
extern int UtcDaliBuiltinConstraintRelativeToConstraint(void);
extern int UtcDaliBuiltinConstraintInverseOfConstraint(void);
extern int UtcDaliBuiltinConstraintFunctions(void);
extern int UtcDaliCustomActorDestructor(void);
extern int UtcDaliCustomActorImplDestructor(void);
extern int UtcDaliCustomActorDownCast(void);
extern int UtcDaliCustomActorDownCastNegative(void);
extern int UtcDaliCustomActorOnStageConnectionDisconnection(void);
extern int UtcDaliCustomActorOnStageConnectionOrder(void);
extern int UtcDaliCustomActorOnStageDisconnectionOrder(void);
extern int UtcDaliCustomActorAddDuringOnStageConnection(void);
extern int UtcDaliCustomActorRemoveDuringOnStageConnection(void);
extern int UtcDaliCustomActorAddDuringOnStageDisconnection(void);
extern int UtcDaliCustomActorRemoveDuringOnStageDisconnection(void);
extern int UtcDaliCustomActorRemoveParentDuringOnStageConnection(void);
extern int UtcDaliCustomActorAddParentDuringOnStageDisconnection(void);
extern int UtcDaliCustomActorOnChildAddRemove(void);
extern int UtcDaliCustomActorReparentDuringOnChildAdd(void);
extern int UtcDaliCustomActorRemoveDuringOnChildRemove(void);
extern int UtcDaliCustomActorOnPropertySet(void);
extern int UtcDaliCustomActorOnSizeSet(void);
extern int UtcDaliCustomActorOnSizeAnimation(void);
extern int UtcDaliCustomActorOnTouchEvent(void);
extern int UtcDaliCustomActorOnMouseWheelEvent(void);
extern int UtcDaliCustomActorFindChildByAlias(void);
extern int UtcDaliCustomActorImplOnPropertySet(void);
extern int UtcDaliCustomActorGetImplementation(void);
extern int UtcDaliDegreeConstructors01(void);
extern int UtcDaliDegreeComparison01(void);
extern int UtcDaliDegreeCastOperators01(void);
extern int UtcDaliDegreeCastOperatorEquals(void);
extern int UtcDaliDegreeCastOperatorNotEquals(void);
extern int UtcDaliDegreeCastOperatorLessThan(void);
extern int UtcDaliInvalidEvent(void);
extern int UtcDaliInvalidGesture(void);
extern int UtcDaliFontNew01(void);
extern int UtcDaliFontNew02(void);
extern int UtcDaliFontNew03(void);
extern int UtcDaliFontNew04(void);
extern int UtcDaliFontNew05(void);
extern int UtcDaliFontNew06(void);
extern int UtcDaliFontDownCast(void);
extern int UtcDaliFontGetPixelSize(void);
extern int UtcDaliFontGetPointSize(void);
extern int UtcDaliFontPointsToPixels(void);
extern int UtcFontMeasureTextWidth(void);
extern int UtcFontMeasureTextHeight(void);
extern int UtcFontMeasureText(void);
extern int UtcFontGetFamilyForText(void);
extern int UtcFontGetFontLineHeightFromCapsHeight(void);
extern int UtcFontAllGlyphsSupported(void);
extern int UtcFontGetMetrics(void);
extern int UtcFontIsDefault(void);
extern int UtcFontGetInstalledFonts(void);
extern int UtcFontMetricsDefaultConstructor(void);
extern int UtcFontMetricsCopyConstructor(void);
extern int UtcFontMetricsAssignmentOperator(void);
extern int UtcDaliFontParamsDefaultConstructor(void);
extern int UtcDaliFontParamsPointSizeConstructor(void);
extern int UtcDaliFontParamsPixelSizeConstructor(void);
extern int UtcDaliFontParamsCopyConstructor(void);
extern int UtcDaliFontParamsAssignmentOperator(void);
extern int UtcDaliFontParamsPointSizeEqualityOperator(void);
extern int UtcDaliFrameBufferImageNew01(void);
extern int UtcDaliFrameBufferImageDownCast(void);
extern int UtcDaliGestureConstructor(void);
extern int UtcDaliGestureAssignment(void);
extern int UtcDaliGestureDetectorConstructorNegative(void);
extern int UtcDaliGestureDetectorConstructorPositive(void);
extern int UtcDaliGestureDetectorDownCast(void);
extern int UtcDaliGestureDetectorAttachPositive(void);
extern int UtcDaliGestureDetectorAttachNegative(void);
extern int UtcDaliGestureDetectorDetachPositive(void);
extern int UtcDaliGestureDetectorDetachNegative01(void);
extern int UtcDaliGestureDetectorDetachNegative02(void);
extern int UtcDaliGestureDetectorDetachNegative03(void);
extern int UtcDaliGestureDetectorDetachAll(void);
extern int UtcDaliGestureDetectorDetachAllNegative(void);
extern int UtcDaliGestureDetectorGetAttachedActors(void);
extern int UtcDaliHandleConstructorVoid(void);
extern int UtcDaliHandleCopyConstructor(void);
extern int UtcDaliHandleAssignmentOperator(void);
extern int UtcDaliHandleSupports(void);
extern int UtcDaliHandleGetPropertyCount(void);
extern int UtcDaliHandleGetPropertyName(void);
extern int UtcDaliHandleGetPropertyIndex(void);
extern int UtcDaliHandleIsPropertyWritable(void);
extern int UtcDaliHandleIsPropertyAnimatable(void);
extern int UtcDaliHandleGetPropertyType(void);
extern int UtcDaliHandleNonAnimtableProperties(void);
extern int UtcDaliHandleNonAnimtableCompositeProperties(void);
extern int UtcDaliHandleSetProperty01(void);
extern int UtcDaliHandleSetProperty02(void);
extern int UtcDaliHandleRegisterProperty(void);
extern int UtcDaliHandleGetProperty(void);
extern int UtcDaliHandleDownCast(void);
extern int UtcDaliHandleCreateProperty(void);
extern int UtcDaliHandleGetPropertyGet(void);
extern int UtcDaliHandleGetPropertyIndices(void);
extern int UtcDaliImageNew01(void);
extern int UtcDaliImageNew02(void);
extern int UtcDaliImageNew03(void);
extern int UtcDaliImageNewWithPolicies01(void);
extern int UtcDaliImageNewWithPolicies02(void);
extern int UtcDaliImageNewWithPolicies03(void);
extern int UtcDaliImageNewWithPolicies04(void);
extern int UtcDaliImageNewDistanceField(void);
extern int UtcDaliImageNewDistanceFieldWithPolicies01(void);
extern int UtcDaliImageNewDistanceFieldWithPolicies02(void);
extern int UtcDaliImageNewDistanceFieldWithPolicies03(void);
extern int UtcDaliImageNewDistanceFieldWithPolicies04(void);
extern int UtcDaliImageNewDistanceFieldWithAttributes(void);
extern int UtcDaliImageNewDistanceFieldWithAttrandPol(void);
extern int UtcDaliImageDownCast(void);
extern int UtcDaliImageGetImageSize(void);
extern int UtcDaliImageGetFilename(void);
extern int UtcDaliImageGetLoadingState01(void);
extern int UtcDaliImageGetLoadingState02(void);
extern int UtcDaliImageGetReleasePolicy(void);
extern int UtcDaliImageGetLoadPolicy(void);
extern int UtcDaliImageSignalLoadingFinished(void);
extern int UtcDaliImageSignalUploaded(void);
extern int UtcDaliImageDiscard01(void);
extern int UtcDaliImageDiscard02(void);
extern int UtcDaliImageDiscard03(void);
extern int UtcDaliImageActorConstructorVoid(void);
extern int UtcDaliImageActorDestructor(void);
extern int UtcDaliImageActorNew01(void);
extern int UtcDaliImageActorNew02(void);
extern int UtcDaliImageActorDownCast(void);
extern int UtcDaliImageActorDownCast2(void);
extern int UtcDaliImageActor9Patch(void);
extern int UtcDaliImageActorPixelArea(void);
extern int UtcDaliImageActorGetCurrentImageSize01(void);
extern int UtcDaliImageActorGetCurrentImageSize02(void);
extern int UtcDaliImageActorGetCurrentImageSize03(void);
extern int UtcDaliImageActorGetCurrentImageSize04(void);
extern int UtcDaliImageActorGetCurrentImageSize05(void);
extern int UtcDaliImageActorNaturalPixelAreaSize01(void);
extern int UtcDaliImageActorNaturalPixelAreaSize02(void);
extern int UtcDaliImageActorDefaultProperties(void);
extern int UtcDaliImageActorUseImageAlpha01(void);
extern int UtcDaliImageActorUseImageAlpha02(void);
extern int UtcDaliImageActorUseImageAlpha03(void);
extern int UtcDaliImageActorUseImageAlpha04(void);
extern int UtcDaliImageActorUseImageAlpha05(void);
extern int UtcDaliImageActorClearPixelArea(void);
extern int UtcDaliImageGetStyle(void);
extern int UtcDaliImageSetNinePatchBorder(void);
extern int UtcDaliImageSetFadeIn(void);
extern int UtcDaliImageSetFadeInDuration(void);
extern int UtcDaliImageActorNewNull(void);
extern int UtcDaliImageActorNewNullWithArea(void);
extern int UtcDaliImageActorSetImage(void);
extern int UtcDaliImageActorPropertyIndices(void);
extern int UtcDaliImageAttributesConstructor(void);
extern int UtcDaliImageAttributesLessThan(void);
extern int UtcDaliImageAttributesEquality(void);
extern int UtcDaliImageAttributesInEquality(void);
extern int UtcDaliKeyEventConstructor(void);
extern int UtcDaliKeyEventIsShiftModifier(void);
extern int UtcDaliKeyEventIsCtrlModifier(void);
extern int UtcDaliKeyEventIsAltModifier(void);
extern int UtcDaliKeyEventIsNotShiftModifier(void);
extern int UtcDaliKeyEventIsNotCtrlModifier(void);
extern int UtcDaliKeyEventIsNotAltModifier(void);
extern int UtcDaliKeyEventANDModifer(void);
extern int UtcDaliKeyEventORModifer(void);
extern int UtcDaliKeyEventState(void);
extern int UtcDaliIntegrationKeyEvent(void);
extern int UtcDaliLayerNew(void);
extern int UtcDaliLayerDownCast(void);
extern int UtcDaliLayerDownCast2(void);
extern int UtcDaliLayerGetDepth(void);
extern int UtcDaliLayerRaise(void);
extern int UtcDaliLayerLower(void);
extern int UtcDaliLayerRaiseToTop(void);
extern int UtcDaliLayerLowerToBottom(void);
extern int UtcDaliLayerSetClipping(void);
extern int UtcDaliLayerIsClipping(void);
extern int UtcDaliLayerSetClippingBox(void);
extern int UtcDaliLayerGetClippingBox(void);
extern int UtcDaliLayerSetSortFunction(void);
extern int UtcDaliLayerRaiseAbove(void);
extern int UtcDaliLayerRaiseBelow(void);
extern int UtcDaliLayerMoveAbove(void);
extern int UtcDaliLayerMoveBelow(void);
extern int UtcDaliLayerDefaultProperties(void);
extern int UtcDaliLayerSetDepthTestDisabled(void);
extern int UtcDaliLayerCreateDestroy(void);
extern int UtcDaliLayerPropertyIndices(void);
extern int UtcDaliLongPressGestureConstructor(void);
extern int UtcDaliLongPressGestureAssignment(void);
extern int UtcDaliLongPressGestureDetectorConstructor(void);
extern int UtcDaliLongPressGestureDetectorNew(void);
extern int UtcDaliLongPressGestureDetectorDownCast(void);
extern int UtcDaliLongPressGestureSetTouchesRequired01(void);
extern int UtcDaliLongPressGestureSetTouchesRequired02(void);
extern int UtcDaliLongPressGestureGetMinimumTouchesRequired(void);
extern int UtcDaliLongPressGestureGetMaximumTouchesRequired(void);
extern int UtcDaliLongPressGestureSignalReceptionNegative(void);
extern int UtcDaliLongPressGestureSignalReceptionPositive(void);
extern int UtcDaliLongPressGestureSignalReceptionDetach(void);
extern int UtcDaliLongPressGestureSignalReceptionActorDestroyedDuringLongPress(void);
extern int UtcDaliLongPressGestureSignalReceptionRotatedActor(void);
extern int UtcDaliLongPressGestureSignalReceptionChildHit(void);
extern int UtcDaliLongPressGestureSignalReceptionAttachDetachMany(void);
extern int UtcDaliLongPressGestureSignalReceptionActorBecomesUntouchable(void);
extern int UtcDaliLongPressGestureSignalReceptionMultipleGestureDetectors(void);
extern int UtcDaliLongPressGestureSignalReceptionMultipleDetectorsOnActor(void);
extern int UtcDaliLongPressGestureSignalReceptionDifferentPossible(void);
extern int UtcDaliLongPressGestureEmitIncorrecteStateClear(void);
extern int UtcDaliLongPressGestureEmitIncorrectStateContinuing(void);
extern int UtcDaliLongPressGestureRepeatedState(void);
extern int UtcDaliLongPressGesturePossibleCancelled(void);
extern int UtcDaliLongPressGestureDetachAfterStarted(void);
extern int UtcDaliLongPressGestureActorUnstaged(void);
extern int UtcDaliLongPressGestureActorStagedAndDestroyed(void);
extern int UtcDaliLongPressGestureSystemOverlay(void);
extern int UtcDaliMaterialNew01(void);
extern int UtcDaliMaterialDownCast(void);
extern int UtcDaliMaterialSettersAndGetters(void);
extern int UtcDaliMaterialStage01(void);
extern int UtcDaliMaterialStage01MemCheck(void);
extern int UtcDaliMaterialStage02(void);
extern int UtcDaliMathUtilsNextPowerOfTwo(void);
extern int UtcDaliMathUtilsIsPowerOfTwo(void);
extern int UtcDaliMathUtilsGetRangedEpsilon(void);
extern int UtcDaliMathUtilsRound(void);
extern int UtcDaliMathUtilsClamp(void);
extern int UtcDaliMathUtilsWrapInDomain(void);
extern int UtcDaliMathUtilsShortestDistanceInDomain(void);
extern int UtcDaliMathUtilsEquals(void);
extern int UtcDaliMatrixCtor(void);
extern int UtcDaliMatrixOrthoNormalize0(void);
extern int UtcDaliMatrixOrthoNormalize1(void);
extern int UtcDaliMatrixInvert01(void);
extern int UtcDaliMatrixInvert02(void);
extern int UtcDaliMatrixInvertTransform01(void);
extern int UtcDaliMatrixInvertTransform02(void);
extern int UtcDaliMatrixGetXAxis(void);
extern int UtcDaliMatrixGetYAxis(void);
extern int UtcDaliMatrixGetZAxis(void);
extern int UtcDaliMatrixGetTranslation(void);
extern int UtcDaliMatrixGetTranslation3(void);
extern int UtcDaliMatrixSetIdentity(void);
extern int UtcDaliMatrixSetIdentityAndScale(void);
extern int UtcDaliMatrixSetXAxis(void);
extern int UtcDaliMatrixSetYAxis(void);
extern int UtcDaliMatrixSetZAxis(void);
extern int UtcDaliMatrixSetTranslation(void);
extern int UtcDaliMatrixSetTranslation3(void);
extern int UtcDaliMatrixTranspose(void);
extern int UtcDaliMatrixOStreamOperator(void);
extern int UtcDaliMatrixMultiply(void);
extern int UtcDaliMatrixOperatorMultiply(void);
extern int UtcDaliMatrixOperatorMultiply02(void);
extern int UtcDaliMatrixOperatorEquals(void);
extern int UtcDaliMatrixOperatorNotEquals(void);
extern int UtcDaliMatrixGetTransformComponents01(void);
extern int UtcDaliMatrixGetTransformComponents02(void);
extern int UtcDaliMatrixSetTransformComponents01(void);
extern int UtcDaliMatrixSetInverseTransformComponent01(void);
extern int UtcDaliMatrixSetInverseTransformComponent02(void);
extern int UtcDaliMatrix3FromMatrix(void);
extern int UtcDaliMatrix3OperatorAssign01(void);
extern int UtcDaliMatrix3OperatorAssign02(void);
extern int UtcDaliMatrix3AsFloat(void);
extern int UtcDaliMatrix3Invert(void);
extern int UtcDaliMatrix3Transpose(void);
extern int UtcDaliMatrix3SetIdentity(void);
extern int UtcDaliMatrix3Scale(void);
extern int UtcDaliMatrix3Magnitude(void);
extern int UtcDaliMatrix3ScaleInverseTranspose(void);
extern int UtcDaliMatrix3OStreamOperator(void);
extern int UtcDaliMatrix3Multiply(void);
extern int UtcDaliMatrix3EqualityOperator(void);
extern int UtcDaliMatrix3InequalityOperator(void);
extern int UtcDaliMeshActorConstructorVoid(void);
extern int UtcDaliMeshActorNew01(void);
extern int UtcDaliMeshActorIndices(void);
extern int UtcDaliPanGestureConstructor(void);
extern int UtcDaliPanGestureAssignment(void);
extern int UtcDaliPanGestureGetSpeed(void);
extern int UtcDaliPanGestureGetDistance(void);
extern int UtcDaliPanGestureGetScreenSpeed(void);
extern int UtcDaliPanGestureGetScreenDistance(void);
extern int UtcDaliPanGestureDetectorConstructor(void);
extern int UtcDaliPanGestureDetectorNew(void);
extern int UtcDaliPanGestureDetectorDownCast(void);
extern int UtcDaliPanGestureSetMinimumTouchesRequired(void);
extern int UtcDaliPanGestureSetMaximumTouchesRequired(void);
extern int UtcDaliPanGestureGetMinimumTouchesRequired(void);
extern int UtcDaliPanGestureGetMaximumTouchesRequired(void);
extern int UtcDaliPanGestureSignalReceptionNegative(void);
extern int UtcDaliPanGestureSignalReceptionDownMotionLeave(void);
extern int UtcDaliPanGestureSignalReceptionDownMotionUp(void);
extern int UtcDaliPanGestureSignalReceptionCancelled(void);
extern int UtcDaliPanGestureSignalReceptionDetach(void);
extern int UtcDaliPanGestureSignalReceptionDetachWhilePanning(void);
extern int UtcDaliPanGestureSignalReceptionActorDestroyedWhilePanning(void);
extern int UtcDaliPanGestureSignalReceptionRotatedActor(void);
extern int UtcDaliPanGestureSignalReceptionChildHit(void);
extern int UtcDaliPanGestureSignalReceptionAttachDetachMany(void);
extern int UtcDaliPanGestureSignalReceptionActorBecomesUntouchable(void);
extern int UtcDaliPanGestureSignalReceptionMultipleGestureDetectors(void);
extern int UtcDaliPanGestureSignalReceptionMultipleDetectorsOnActor(void);
extern int UtcDaliPanGestureSignalReceptionMultipleStarted(void);
extern int UtcDaliPanGestureSignalReceptionEnsureCorrectSignalling(void);
extern int UtcDaliPanGestureSignalReceptionDifferentPossible(void);
extern int UtcDaliPanGestureEmitIncorrectState(void);
extern int UtcDaliPanGestureActorUnstaged(void);
extern int UtcDaliPanGestureActorStagedAndDestroyed(void);
extern int UtcDaliPanGestureSystemOverlay(void);
extern int UtcDaliPanGestureAngleHandling(void);
extern int UtcDaliPanGestureAngleOutOfRange(void);
extern int UtcDaliPanGestureAngleProcessing(void);
extern int UtcDaliPanGestureDirectionHandling(void);
extern int UtcDaliPanGestureDirectionProcessing(void);
extern int UtcDaliPanGestureSetProperties(void);
extern int UtcDaliPanGestureSetPropertiesAlreadyPanning(void);
extern int UtcDaliPanGesturePropertyIndices(void);
extern int UtcDaliPinchGestureConstructor(void);
extern int UtcDaliPinchGestureAssignment(void);
extern int UtcDaliPinchGestureDetectorConstructor(void);
extern int UtcDaliPinchGestureDetectorNew(void);
extern int UtcDaliPinchGestureDetectorDownCast(void);
extern int UtcDaliPinchGestureSignalReceptionNegative(void);
extern int UtcDaliPinchGestureSignalReceptionDownMotionLeave(void);
extern int UtcDaliPinchGestureSignalReceptionDownMotionUp(void);
extern int UtcDaliPinchGestureSignalReceptionCancelled(void);
extern int UtcDaliPinchGestureSignalReceptionDetach(void);
extern int UtcDaliPinchGestureSignalReceptionDetachWhilePinching(void);
extern int UtcDaliPinchGestureSignalReceptionActorDestroyedWhilePinching(void);
extern int UtcDaliPinchGestureSignalReceptionRotatedActor(void);
extern int UtcDaliPinchGestureSignalReceptionChildHit(void);
extern int UtcDaliPinchGestureSignalReceptionAttachDetachMany(void);
extern int UtcDaliPinchGestureSignalReceptionActorBecomesUntouchable(void);
extern int UtcDaliPinchGestureSignalReceptionMultipleDetectorsOnActor(void);
extern int UtcDaliPinchGestureSignalReceptionMultipleStarted(void);
extern int UtcDaliPinchGestureSignalReceptionEnsureCorrectSignalling(void);
extern int UtcDaliPinchGestureEmitIncorrectStateClear(void);
extern int UtcDaliPinchGestureEmitIncorrectStatePossible(void);
extern int UtcDaliPinchGestureActorUnstaged(void);
extern int UtcDaliPinchGestureActorStagedAndDestroyed(void);
extern int UtcDaliPinchGestureSystemOverlay(void);
extern int UtcDaliPixelHasAlpha(void);
extern int UtcDaliPixelGetBytesPerPixel(void);
extern int UtcDaliPixelGetAlphaOffsetAndMask(void);
extern int UtcDaliPropertyNotificationDownCast(void);
extern int UtcDaliPropertyNotificationDownCastNegative(void);
extern int UtcDaliAddPropertyNotification(void);
extern int UtcDaliAddPropertyNotificationCallback(void);
extern int UtcDaliAddPropertyNotificationTypeProperty(void);
extern int UtcDaliPropertyNotificationGetCondition(void);
extern int UtcDaliPropertyNotificationGetConditionConst(void);
extern int UtcDaliPropertyNotificationGetTarget(void);
extern int UtcDaliPropertyNotificationGetProperty(void);
extern int UtcDaliPropertyNotificationGetNotifyMode(void);
extern int UtcDaliPropertyNotificationGreaterThan(void);
extern int UtcDaliPropertyNotificationLessThan(void);
extern int UtcDaliPropertyNotificationInside(void);
extern int UtcDaliPropertyNotificationOutside(void);
extern int UtcDaliPropertyNotificationVectorComponentGreaterThan(void);
extern int UtcDaliPropertyNotificationVectorComponentLessThan(void);
extern int UtcDaliPropertyNotificationVectorComponentInside(void);
extern int UtcDaliPropertyNotificationVectorComponentOutside(void);
extern int UtcDaliPropertyConditionGetArguments(void);
extern int UtcDaliPropertyConditionGetArgumentsConst(void);
extern int UtcDaliPropertyNotificationStep(void);
extern int UtcDaliPropertyNotificationVariableStep(void);
extern int UtcDaliQuaternionCtor01(void);
extern int UtcDaliQuaternionCtor02(void);
extern int UtcDaliQuaternionCtor03(void);
extern int UtcDaliQuaternionFromAxisAngle(void);
extern int UtcDaliQuaternionToAxisAngle01(void);
extern int UtcDaliQuaternionToAxisAngle02(void);
extern int UtcDaliQuaternionToAxisAngle03(void);
extern int UtcDaliQuaternionToAxisAngle04(void);
extern int UtcDaliQuaternionEulerAngles(void);
extern int UtcDaliQuaternionToMatrix01(void);
extern int UtcDaliQuaternionToMatrix02(void);
extern int UtcDaliQuaternionFromMatrix01(void);
extern int UtcDaliQuaternionFromMatrix02(void);
extern int UtcDaliQuaternionFromAxes01(void);
extern int UtcDaliQuaternionFromAxes02(void);
extern int UtcDaliQuaternionOperatorAddition(void);
extern int UtcDaliQuaternionOperatorSubtraction(void);
extern int UtcDaliQuaternionConjugate(void);
extern int UtcDaliQuaternionOperatorMultiplication01(void);
extern int UtcDaliQuaternionOperatorDivision(void);
extern int UtcDaliQuaternionOperatorScale01(void);
extern int UtcDaliQuaternionOperatorScale02(void);
extern int UtcDaliQuaternionOperatorNegation(void);
extern int UtcDaliQuaternionOperatorAddAssign(void);
extern int UtcDaliQuaternionOperatorSubtractAssign(void);
extern int UtcDaliQuaternionOperatorMultiplyAssign(void);
extern int UtcDaliQuaternionOperatorScaleAssign01(void);
extern int UtcDaliQuaternionOperatorScaleAssign02(void);
extern int UtcDaliQuaternionOperatorEquality(void);
extern int UtcDaliQuaternionOperatorInequality(void);
extern int UtcDaliQuaternionLength(void);
extern int UtcDaliQuaternionLengthSquared(void);
extern int UtcDaliQuaternionNormalize(void);
extern int UtcDaliQuaternionNormalized(void);
extern int UtcDaliQuaternionInvert(void);
extern int UtcDaliQuaternionDot(void);
extern int UtcDaliQuaternionOperatorMultiplication02(void);
extern int UtcDaliQuaternionRotate01(void);
extern int UtcDaliQuaternionRotate02(void);
extern int UtcDaliQuaternionExp01(void);
extern int UtcDaliQuaternionExp02(void);
extern int UtcDaliQuaternionExp03(void);
extern int UtcDaliQuaternionLog01(void);
extern int UtcDaliQuaternionLog02(void);
extern int UtcDaliQuaternionLerp(void);
extern int UtcDaliQuaternionSlerp01(void);
extern int UtcDaliQuaternionSlerp02(void);
extern int UtcDaliQuaternionSlerp03(void);
extern int UtcDaliQuaternionSlerp04(void);
extern int UtcDaliQuaternionSlerpNoInvert01(void);
extern int UtcDaliQuaternionSlerpNoInvert02(void);
extern int UtcDaliQuaternionSquad(void);
extern int UtcDaliAngleBetween(void);
extern int UtcDaliQuaternionOStreamOperator(void);
extern int UtcDaliRadianConstructors01(void);
extern int UtcDaliRadianComparison01(void);
extern int UtcDaliRadianCastOperators01(void);
extern int UtcDaliRadianCastOperatorEquals(void);
extern int UtcDaliRadianCastOperatorNotEquals(void);
extern int UtcDaliRadianCastOperatorLessThan(void);
extern int UtcDaliRectCons01(void);
extern int UtcDaliRectCons02(void);
extern int UtcDaliRectCons03(void);
extern int UtcDaliRectCons04(void);
extern int UtcDaliRectSet(void);
extern int UtcDaliRectIsEmpty(void);
extern int UtcDaliRectRight(void);
extern int UtcDaliRectBottom(void);
extern int UtcDaliRectArea(void);
extern int UtcDaliRectIntersects(void);
extern int UtcDaliRectContains(void);
extern int UtcDaliRectOperatorNotEquals(void);
extern int UtcDaliRectOperatorEquals(void);
extern int UtcDaliRenderTaskDownCast(void);
extern int UtcDaliRenderTaskSetSourceActor(void);
extern int UtcDaliRenderTaskSetSourceActorOffStage(void);
extern int UtcDaliRenderTaskSetSourceActorEmpty(void);
extern int UtcDaliRenderTaskGetSourceActor(void);
extern int UtcDaliRenderTaskSetExclusive(void);
extern int UtcDaliRenderTaskIsExclusive(void);
extern int UtcDaliRenderTaskSetInputEnabled(void);
extern int UtcDaliRenderTaskGetInputEnabled(void);
extern int UtcDaliRenderTaskSetCameraActor(void);
extern int UtcDaliRenderTaskGetCameraActor(void);
extern int UtcDaliRenderTaskSetTargetFrameBuffer(void);
extern int UtcDaliRenderTaskGetTargetFrameBuffer(void);
extern int UtcDaliRenderTaskSetScreenToFrameBufferFunction(void);
extern int UtcDaliRenderTaskGetScreenToFrameBufferFunction(void);
extern int UtcDaliRenderTaskGetScreenToFrameBufferMappingActor(void);
extern int UtcDaliRenderTaskSetViewport(void);
extern int UtcDaliRenderTaskGetViewport(void);
extern int UtcDaliRenderTaskSetViewportPosition(void);
extern int UtcDaliRenderTaskSetViewportSize(void);
extern int UtcDaliRenderTaskSetClearColor(void);
extern int UtcDaliRenderTaskGetClearColor(void);
extern int UtcDaliRenderTaskSetClearEnabled(void);
extern int UtcDaliRenderTaskGetClearEnabled(void);
extern int UtcDaliRenderTaskSetRefreshRate(void);
extern int UtcDaliRenderTaskGetRefreshRate(void);
extern int UtcDaliRenderTaskSignalFinished(void);
extern int UtcDaliRenderTaskContinuous01(void);
extern int UtcDaliRenderTaskContinuous02(void);
extern int UtcDaliRenderTaskContinuous03(void);
extern int UtcDaliRenderTaskContinuous04(void);
extern int UtcDaliRenderTaskContinuous05(void);
extern int UtcDaliRenderTaskContinuous06(void);
extern int UtcDaliRenderTaskOnce01(void);
extern int UtcDaliRenderTaskOnce02(void);
extern int UtcDaliRenderTaskOnce03(void);
extern int UtcDaliRenderTaskOnce05(void);
extern int UtcDaliRenderTaskOnce06(void);
extern int UtcDaliRenderTaskOnce07(void);
extern int UtcDaliRenderTaskOnce08(void);
extern int UtcDaliRenderTaskOnceNoSync01(void);
extern int UtcDaliRenderTaskOnceNoSync02(void);
extern int UtcDaliRenderTaskOnceNoSync03(void);
extern int UtcDaliRenderTaskOnceNoSync05(void);
extern int UtcDaliRenderTaskOnceNoSync06(void);
extern int UtcDaliRenderTaskOnceNoSync07(void);
extern int UtcDaliRenderTaskOnceNoSync08(void);
extern int UtcDaliRenderTaskOnceChain01(void);
extern int UtcDaliRenderTaskProperties(void);
extern int UtcDaliRenderTaskListDefaultConstructor(void);
extern int UtcDaliRenderTaskListDownCast(void);
extern int UtcDaliRenderTaskListCreateTask(void);
extern int UtcDaliRenderTaskListRemoveTask(void);
extern int UtcDaliRenderTaskListGetTaskCount(void);
extern int UtcDaliRenderTaskListGetTask(void);
extern int UtcDaliRenderableActorDownCast(void);
extern int UtcDaliRenderableActorSetSortModifier(void);
extern int UtcDaliRenderableActorGetSortModifier(void);
extern int UtcDaliRenderableActorSetGetBlendMode(void);
extern int UtcDaliRenderableActorSetCullFace(void);
extern int UtcDaliRenderableActorGetCullFace(void);
extern int UtcDaliRenderableActorSetGetBlendFunc(void);
extern int UtcDaliRenderableActorSetGetBlendEquation(void);
extern int UtcDaliRenderableActorSetGetBlendColor(void);
extern int UtcDaliRenderableActorSetGetAlpha(void);
extern int UtcDaliRenderableActorCreateDestroy(void);
extern int UtcDaliShaderEffectMethodNew01(void);
extern int UtcDaliShaderEffectMethodNew02(void);
extern int UtcDaliShaderEffectMethodNew03(void);
extern int UtcDaliShaderEffectMethodNew04(void);
extern int UtcDaliShaderEffectMethodNew05(void);
extern int UtcDaliShaderEffectMethodNew06(void);
extern int UtcDaliShaderEffectMethodDownCast(void);
extern int UtcDaliShaderEffectMethodDelete01(void);
extern int UtcDaliShaderEffectMethodSetUniformFloat(void);
extern int UtcDaliShaderEffectMethodSetUniformVector2(void);
extern int UtcDaliShaderEffectMethodSetUniformVector3(void);
extern int UtcDaliShaderEffectMethodSetUniformVector4(void);
extern int UtcDaliShaderEffectMethodSetUniformMatrix(void);
extern int UtcDaliShaderEffectMethodSetUniformMatrix3(void);
extern int UtcDaliShaderEffectMethodSetUniformViewport(void);
extern int UtcDaliShaderEffectMethodSetEffectImage(void);
extern int UtcDaliShaderEffectMethodSetEffectImageAndDelete(void);
extern int UtcDaliShaderEffectMethodApplyConstraint(void);
extern int UtcDaliShaderEffectMethodApplyConstraintFromActor(void);
extern int UtcDaliShaderEffectMethodApplyConstraintFromActor2(void);
extern int UtcDaliShaderEffectMethodApplyConstraintCallback(void);
extern int UtcDaliShaderEffectMethodRemoveConstraints(void);
extern int UtcDaliShaderEffectMethodRemoveConstraints2(void);
extern int UtcDaliShaderEffectMethodCreateExtension(void);
extern int UtcDaliShaderEffectMethodCreateExtension2(void);
extern int UtcDaliShaderEffectMethodNoExtension(void);
extern int UtcDaliShaderEffectPropertyIndices(void);
extern int UtcDaliSignalFunctorsEmptyCheck(void);
extern int UtcDaliSignalFunctorsEmit(void);
extern int UtcDaliSignalFunctorsEmitReturn(void);
extern int UtcDaliSignalFunctorsDisconnectBeforeEmit(void);
extern int UtcDaliSignalFunctorsDestroySignal(void);
extern int UtcDaliSignalConnectVoidFunctor(void);
extern int UtcDaliSignalEmptyCheck(void);
extern int UtcDaliSignalEmptyCheckSlotDestruction(void);
extern int UtcDaliSignalConnectAndEmit(void);
extern int UtcDaliSignalDisconnect(void);
extern int UtcDaliSignalDisconnect2(void);
extern int UtcDaliSignalDisconnect3(void);
extern int UtcDaliSignalCustomConnectionTracker(void);
extern int UtcDaliSignalMultipleConnections(void);
extern int UtcDaliSignalMultipleConnections2(void);
extern int UtcDaliSignalMultipleConnections3(void);
extern int UtcDaliSignalDisconnectStatic(void);
extern int UtcDaliSignalDisconnectDuringCallback(void);
extern int UtcDaliSignalDisconnectDuringCallback2(void);
extern int UtcDaliSignalEmitDuringCallback(void);
extern int UtcDaliSignalTestApp01(void);
extern int UtcDaliSignalTestApp02(void);
extern int UtcDaliSignalTestApp03(void);
extern int UtcDaliSignalTestApp04(void);
extern int UtcDaliSignalTestApp05(void);
extern int UtcDaliSignalTestApp06(void);
extern int UtcDaliSlotDelegateConnection(void);
extern int UtcDaliSignalSlotDelegateDestruction(void);
extern int UtcDaliSlotHandlerDisconnect(void);
extern int UtcDaliStageDefaultConstructor(void);
extern int UtcDaliStageDestructor(void);
extern int UtcDaliStageGetCurrent(void);
extern int UtcDaliStageIsInstalled(void);
extern int UtcDaliStageAdd(void);
extern int UtcDaliStageRemove(void);
extern int UtcDaliStageGetSize(void);
extern int UtcDaliStageGetDpi01(void);
extern int UtcDaliStageGetDpi02(void);
extern int UtcDaliStageGetDpi03(void);
extern int UtcDaliStageGetLayerCount(void);
extern int UtcDaliStageGetLayer(void);
extern int UtcDaliStageGetRootLayer(void);
extern int UtcDaliStageSetBackgroundColor(void);
extern int UtcDaliStageGetBackgroundColor(void);
extern int UtcDaliStageKeepRendering(void);
extern int UtcDaliStageEventProcessingFinished(void);
extern int UtcDaliStageSignalKeyEvent(void);
extern int UtcDaliStageTouchedSignal(void);
extern int UtcDaliTapGestureConstructor(void);
extern int UtcDaliTapGestureAssignment(void);
extern int UtcDaliTapGestureDetectorConstructor(void);
extern int UtcDaliTapGestureDetectorNew(void);
extern int UtcDaliTapGestureDetectorDownCast(void);
extern int UtcDaliTapGestureSetTapsRequired(void);
extern int UtcDaliTapGestureGetTapsRequired(void);
extern int UtcDaliTapGestureSetTouchesRequired(void);
extern int UtcDaliTapGestureGetTouchesRequired(void);
extern int UtcDaliTapGestureSignalReceptionNegative(void);
extern int UtcDaliTapGestureSignalReceptionPositive(void);
extern int UtcDaliTapGestureSignalReceptionDetach(void);
extern int UtcDaliTapGestureSignalReceptionActorDestroyedWhileTapping(void);
extern int UtcDaliTapGestureSignalReceptionRotatedActor(void);
extern int UtcDaliTapGestureSignalReceptionChildHit(void);
extern int UtcDaliTapGestureSignalReceptionAttachDetachMany(void);
extern int UtcDaliTapGestureSignalReceptionActorBecomesUntouchable(void);
extern int UtcDaliTapGestureSignalReceptionMultipleGestureDetectors(void);
extern int UtcDaliTapGestureSignalReceptionMultipleDetectorsOnActor(void);
extern int UtcDaliTapGestureSignalReceptionDifferentPossible(void);
extern int UtcDaliTapGestureEmitIncorrectStateClear(void);
extern int UtcDaliTapGestureEmitIncorrectStateContinuing(void);
extern int UtcDaliTapGestureEmitIncorrectStateFinished(void);
extern int UtcDaliTapGestureActorUnstaged(void);
extern int UtcDaliTapGestureRepeatedState(void);
extern int UtcDaliTapGesturePossibleCancelled(void);
extern int UtcDaliTapGestureDetectorRemovedWhilePossible(void);
extern int UtcDaliTapGestureActorRemovedWhilePossible(void);
extern int UtcDaliTapGestureSystemOverlay(void);
extern int UtcDaliTextConstructor(void);
extern int UtcDaliTextCopyConstructor(void);
extern int UtcDaliTextAssignmentOperator(void);
extern int UtcDaliTextSetGetText(void);
extern int UtcDaliTextAccessOperator01(void);
extern int UtcDaliTextAccessOperator02(void);
extern int UtcDaliTextIsEmpty(void);
extern int UtcDaliTextGetLength(void);
extern int UtcDaliTextAppend(void);
extern int UtcDaliTextRemove01(void);
extern int UtcDaliTextRemove02(void);
extern int UtcDaliTextActorConstructorVoid(void);
extern int UtcDaliTextActorNew01(void);
extern int UtcDaliTextActorNew02(void);
extern int UtcDaliTextActorNew03(void);
extern int UtcDaliTextActorNew04(void);
extern int UtcDaliTextActorNew05(void);
extern int UtcDaliTextActorNew06(void);
extern int UtcDaliTextActorNew07(void);
extern int UtcDaliTextActorDownCast(void);
extern int UtcDaliTextActorDownCast2(void);
extern int UtcDaliTextActorSetText(void);
extern int UtcDaliTextActorSetFont(void);
extern int UtcDaliTextActorSetFontDetection(void);
extern int UtcDaliTextActorSetTextIndividualStyles(void);
extern int UtcDaliTextActorChangingText(void);
extern int UtcDaliTextActorGetLoadingState(void);
extern int UtcDaliTextActorSetItalics(void);
extern int UtcDaliTextActorSetUnderline(void);
extern int UtcDaliTextActorSetWeight(void);
extern int UtcDaliTextActorSetStyle(void);
extern int UtcDaliTextActorDefaultProperties(void);
extern int UtcDaliTextActorSetGradientColor(void);
extern int UtcDaliTextActorSetGradientStartPoint(void);
extern int UtcDaliTextActorSetGradientEndPoint(void);
extern int UtcDaliTextActorSynchronousGlyphLoading(void);
extern int UtcDaliTextActorAutomaticSizeSet(void);
extern int UtcDaliTextActorAutomaticSizeSetAnimation(void);
extern int UtcDaliTextActorPropertyIndices(void);
extern int UtcDaliTextStyleDefaultConstructor(void);
extern int UtcDaliTextStyleCopyConstructor(void);
extern int UtcDaliTextStyleComparisonOperator(void);
extern int UtcDaliTextStyleCopy(void);
extern int UtcDaliTextStyleSetGetFontName(void);
extern int UtcDaliTextStyleSetGetFontStyle(void);
extern int UtcDaliTextStyleSetGetFontPointSize(void);
extern int UtcDaliTextStyleSetGetWeight(void);
extern int UtcDaliTextStyleSetGetTextColor(void);
extern int UtcDaliTextStyleSetGetItalics(void);
extern int UtcDaliTextStyleSetGetItalicsAngle(void);
extern int UtcDaliTextStyleSetGetUnderline(void);
extern int UtcDaliTextStyleSetGetShadow(void);
extern int UtcDaliTextStyleSetGetGlow(void);
extern int UtcDaliTextStyleSetGetOutline(void);
extern int UtcDaliTextStyleSetGetSmoothEdge(void);
extern int UtcDaliTouchEventCombinerConstructors(void);
extern int UtcDaliTouchEventCombinerConstructorsNegative(void);
extern int UtcDaliTouchEventCombinerSettersAndGetters(void);
extern int UtcDaliTouchEventCombinerSettersNegative(void);
extern int UtcDaliTouchEventCombinerSingleTouchNormal(void);
extern int UtcDaliTouchEventCombinerSingleTouchMotionWithoutDown(void);
extern int UtcDaliTouchEventCombinerSingleTouchTwoDowns(void);
extern int UtcDaliTouchEventCombinerSingleTouchUpWithoutDown(void);
extern int UtcDaliTouchEventCombinerSingleTouchTwoUps(void);
extern int UtcDaliTouchEventCombinerSingleTouchUpWithDifferentId(void);
extern int UtcDaliTouchEventCombinerSingleTouchMotionWithDifferentId(void);
extern int UtcDaliTouchEventCombinerMultiTouchNormal(void);
extern int UtcDaliTouchEventCombinerSeveralPoints(void);
extern int UtcDaliTouchEventCombinerReset(void);
extern int UtcDaliTouchEventCombinerSingleTouchInterrupted(void);
extern int UtcDaliTouchEventCombinerMultiTouchInterrupted(void);
extern int UtcDaliTouchEventCombinerInvalidState(void);
extern int UtcDaliTouchNormalProcessing(void);
extern int UtcDaliTouchOutsideCameraNearFarPlanes(void);
extern int UtcDaliTouchEmitEmpty(void);
extern int UtcDaliTouchInterrupted(void);
extern int UtcDaliTouchParentConsumer(void);
extern int UtcDaliTouchInterruptedParentConsumer(void);
extern int UtcDaliTouchLeave(void);
extern int UtcDaliTouchLeaveParentConsumer(void);
extern int UtcDaliTouchActorBecomesInsensitive(void);
extern int UtcDaliTouchActorBecomesInsensitiveParentConsumer(void);
extern int UtcDaliTouchMultipleLayers(void);
extern int UtcDaliTouchMultipleRenderTasks(void);
extern int UtcDaliTouchMultipleRenderTasksWithChildLayer(void);
extern int UtcDaliTouchOffscreenRenderTasks(void);
extern int UtcDaliTouchMultipleRenderableActors(void);
extern int UtcDaliTouchActorRemovedInSignal(void);
extern int UtcDaliTouchActorSignalNotConsumed(void);
extern int UtcDaliTouchActorUnStaged(void);
extern int UtcDaliTouchSystemOverlayActor(void);
extern int UtcDaliVector2Cons(void);
extern int UtcDaliVector2FitInside(void);
extern int UtcDaliVector2FitScaleToFill(void);
extern int UtcDaliVector2ShrinkInside(void);
extern int UtcDaliVector2Add(void);
extern int UtcDaliVector2Subtract(void);
extern int UtcDaliVector2Negate(void);
extern int UtcDaliVector2Multiply(void);
extern int UtcDaliVector2Divide(void);
extern int UtcDaliVector2Scale(void);
extern int UtcDaliVector2Equals(void);
extern int UtcDaliVector2Length(void);
extern int UtcDaliVector2LengthSquared(void);
extern int UtcDaliVector2Max(void);
extern int UtcDaliVector2Min(void);
extern int UtcDaliVector2Clamp(void);
extern int UtcDaliVector2ClampVector2(void);
extern int UtcDaliVector2Normalize(void);
extern int UtcDaliVector2OperatorSubscript(void);
extern int UtcDaliVector2OStreamOperator(void);
extern int UtcDaliVector3Cons(void);
extern int UtcDaliVector3Assign(void);
extern int UtcDaliVector3Add(void);
extern int UtcDaliVector3Constants(void);
extern int UtcDaliVector3Cross(void);
extern int UtcDaliVector3Dot(void);
extern int UtcDaliVector3Equals(void);
extern int UtcDaliVector3Length(void);
extern int UtcDaliVector3LengthSquared(void);
extern int UtcDaliVector3Max(void);
extern int UtcDaliVector3Min(void);
extern int UtcDaliVector3Clamp(void);
extern int UtcDaliVector3ClampVector3(void);
extern int UtcDaliVector3Multiply(void);
extern int UtcDaliVector3Divide(void);
extern int UtcDaliVector3Scale(void);
extern int UtcDaliVector3Normalize(void);
extern int UtcDaliVector3Subtract(void);
extern int UtcDaliVector3OperatorSubscript(void);
extern int UtcDaliVector3OStreamOperator(void);
extern int UtcDaliVector3Rotate(void);
extern int UtcDaliVector3AsFloat(void);
extern int UtcDaliVector3AsVectorXY(void);
extern int UtcDaliVector3FitKeepAspectRatio(void);
extern int UtcDaliVector3FillKeepAspectRatio(void);
extern int UtcDaliVector3FillXYKeepAspectRatio(void);
extern int UtcDaliVector3ShrinkInsideKeepAspectRatio(void);
extern int UtcDaliVector4Vector4(void);
extern int UtcDaliVector4Add(void);
extern int UtcDaliVector4Constants(void);
extern int UtcDaliVector4Cross(void);
extern int UtcDaliVector4Dot(void);
extern int UtcDaliVector4DotVector3(void);
extern int UtcDaliVector4Dot4(void);
extern int UtcDaliVector4Equals(void);
extern int UtcDaliVector4Length(void);
extern int UtcDaliVector4LengthSquared(void);
extern int UtcDaliVector4Max(void);
extern int UtcDaliVector4Min(void);
extern int UtcDaliVector4Clamp(void);
extern int UtcDaliVector4ClampVector4(void);
extern int UtcDaliVector4Multiply(void);
extern int UtcDaliVector4Divide(void);
extern int UtcDaliVector4Scale(void);
extern int UtcDaliVector4Normalize(void);
extern int UtcDaliVector4Subtract(void);
extern int UtcDaliVector4OperatorSubscript(void);
extern int UtcDaliVector4OStreamOperator(void);
extern int UtcDaliVector4AsFloat(void);

testcase tc_array[] = {
    {"UtcDaliConstraintGetTargetObject", UtcDaliConstraintGetTargetObject, utc_dali_active_constraint_startup, utc_dali_active_constraint_cleanup},
    {"UtcDaliConstraintGetTargetProperty", UtcDaliConstraintGetTargetProperty, utc_dali_active_constraint_startup, utc_dali_active_constraint_cleanup},
    {"UtcDaliConstraintSetWeight", UtcDaliConstraintSetWeight, utc_dali_active_constraint_startup, utc_dali_active_constraint_cleanup},
    {"UtcDaliConstraintGetCurrentWeight", UtcDaliConstraintGetCurrentWeight, utc_dali_active_constraint_startup, utc_dali_active_constraint_cleanup},
    {"UtcDaliConstraintSignalApplied", UtcDaliConstraintSignalApplied, utc_dali_active_constraint_startup, utc_dali_active_constraint_cleanup},
    {"UtcDaliConstraintRemove", UtcDaliConstraintRemove, utc_dali_active_constraint_startup, utc_dali_active_constraint_cleanup},
    {"UtcDaliConstraintCallback", UtcDaliConstraintCallback, utc_dali_active_constraint_startup, utc_dali_active_constraint_cleanup},
    {"UtcDaliConstraintProperties", UtcDaliConstraintProperties, utc_dali_active_constraint_startup, utc_dali_active_constraint_cleanup},
    {"UtcDaliActorNew", UtcDaliActorNew, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorDownCast", UtcDaliActorDownCast, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorDownCast2", UtcDaliActorDownCast2, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetName", UtcDaliActorGetName, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetName", UtcDaliActorSetName, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetId", UtcDaliActorGetId, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorIsRoot", UtcDaliActorIsRoot, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorOnStage", UtcDaliActorOnStage, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorIsLayer", UtcDaliActorIsLayer, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetLayer", UtcDaliActorGetLayer, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorAdd", UtcDaliActorAdd, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorRemove01", UtcDaliActorRemove01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorRemove02", UtcDaliActorRemove02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetChildCount", UtcDaliActorGetChildCount, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetChildren01", UtcDaliActorGetChildren01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetChildren02", UtcDaliActorGetChildren02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetParent01", UtcDaliActorGetParent01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetParent02", UtcDaliActorGetParent02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetParentOrigin", UtcDaliActorSetParentOrigin, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentParentOrigin", UtcDaliActorGetCurrentParentOrigin, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetAnchorPoint", UtcDaliActorSetAnchorPoint, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentAnchorPoint", UtcDaliActorGetCurrentAnchorPoint, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetSize01", UtcDaliActorSetSize01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetSize02", UtcDaliActorSetSize02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetSize03", UtcDaliActorSetSize03, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetSize04", UtcDaliActorSetSize04, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentSize", UtcDaliActorGetCurrentSize, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetPosition01", UtcDaliActorSetPosition01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetPosition02", UtcDaliActorSetPosition02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetPosition03", UtcDaliActorSetPosition03, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetX", UtcDaliActorSetX, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetY", UtcDaliActorSetY, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetZ", UtcDaliActorSetZ, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorMoveBy", UtcDaliActorMoveBy, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentPosition", UtcDaliActorGetCurrentPosition, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentWorldPosition", UtcDaliActorGetCurrentWorldPosition, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorInheritPosition", UtcDaliActorInheritPosition, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetRotation01", UtcDaliActorSetRotation01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetRotation02", UtcDaliActorSetRotation02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorRotateBy01", UtcDaliActorRotateBy01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorRotateBy02", UtcDaliActorRotateBy02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentRotation", UtcDaliActorGetCurrentRotation, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentWorldRotation", UtcDaliActorGetCurrentWorldRotation, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetScale01", UtcDaliActorSetScale01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetScale02", UtcDaliActorSetScale02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetScale03", UtcDaliActorSetScale03, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorScaleBy", UtcDaliActorScaleBy, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentScale", UtcDaliActorGetCurrentScale, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentWorldScale", UtcDaliActorGetCurrentWorldScale, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorInheritScale", UtcDaliActorInheritScale, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetVisible", UtcDaliActorSetVisible, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorIsVisible", UtcDaliActorIsVisible, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetOpacity", UtcDaliActorSetOpacity, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentOpacity", UtcDaliActorGetCurrentOpacity, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetSensitive", UtcDaliActorSetSensitive, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorIsSensitive", UtcDaliActorIsSensitive, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetInheritShaderEffect", UtcDaliActorSetInheritShaderEffect, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetInheritShaderEffect", UtcDaliActorGetInheritShaderEffect, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetShaderEffect", UtcDaliActorSetShaderEffect, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetShaderEffect", UtcDaliActorGetShaderEffect, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorRemoveShaderEffect01", UtcDaliActorRemoveShaderEffect01, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorRemoveShaderEffect02", UtcDaliActorRemoveShaderEffect02, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetColor", UtcDaliActorSetColor, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentColor", UtcDaliActorGetCurrentColor, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentWorldColor", UtcDaliActorGetCurrentWorldColor, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetColorMode", UtcDaliActorSetColorMode, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorScreenToLocal", UtcDaliActorScreenToLocal, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetLeaveRequired", UtcDaliActorSetLeaveRequired, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetLeaveRequired", UtcDaliActorGetLeaveRequired, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetKeyboardFocusable", UtcDaliActorSetKeyboardFocusable, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorIsKeyboardFocusable", UtcDaliActorIsKeyboardFocusable, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorApplyConstraint", UtcDaliActorApplyConstraint, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorApplyConstraintAppliedCallback", UtcDaliActorApplyConstraintAppliedCallback, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorRemoveConstraints", UtcDaliActorRemoveConstraints, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorRemoveConstraint", UtcDaliActorRemoveConstraint, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorTouchedSignal", UtcDaliActorTouchedSignal, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetSizeSignal", UtcDaliActorSetSizeSignal, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorOnOffStageSignal", UtcDaliActorOnOffStageSignal, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorFindChildByName", UtcDaliActorFindChildByName, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorFindChildByAlias", UtcDaliActorFindChildByAlias, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorFindChildById", UtcDaliActorFindChildById, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorHitTest", UtcDaliActorHitTest, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetDrawMode", UtcDaliActorSetDrawMode, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetDrawModeOverlayRender", UtcDaliActorSetDrawModeOverlayRender, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetDrawModeOverlayHitTest", UtcDaliActorSetDrawModeOverlayHitTest, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetCurrentWorldMatrix", UtcDaliActorGetCurrentWorldMatrix, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorConstrainedToWorldMatrix", UtcDaliActorConstrainedToWorldMatrix, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorUnparent", UtcDaliActorUnparent, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorGetChildAt", UtcDaliActorGetChildAt, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorSetGetOverlay", UtcDaliActorSetGetOverlay, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorDynamics", UtcDaliActorDynamics, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliActorCreateDestroy", UtcDaliActorCreateDestroy, utc_dali_actor_startup, utc_dali_actor_cleanup},
    {"UtcDaliAlphaFunctionsDefault", UtcDaliAlphaFunctionsDefault, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsLinear", UtcDaliAlphaFunctionsLinear, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsReverse", UtcDaliAlphaFunctionsReverse, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseIn", UtcDaliAlphaFunctionsEaseIn, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseOut", UtcDaliAlphaFunctionsEaseOut, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseInOut", UtcDaliAlphaFunctionsEaseInOut, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseInSine", UtcDaliAlphaFunctionsEaseInSine, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseOutSine", UtcDaliAlphaFunctionsEaseOutSine, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseInOutSine", UtcDaliAlphaFunctionsEaseInOutSine, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsBounce", UtcDaliAlphaFunctionsBounce, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsBounceBack", UtcDaliAlphaFunctionsBounceBack, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseOutBack", UtcDaliAlphaFunctionsEaseOutBack, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsSin", UtcDaliAlphaFunctionsSin, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsSin2x", UtcDaliAlphaFunctionsSin2x, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsSquare", UtcDaliAlphaFunctionsSquare, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseInSine33", UtcDaliAlphaFunctionsEaseInSine33, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseOutSine33", UtcDaliAlphaFunctionsEaseOutSine33, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseInOutSineXX", UtcDaliAlphaFunctionsEaseInOutSineXX, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsDoubleEaseInOutSine60", UtcDaliAlphaFunctionsDoubleEaseInOutSine60, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseOutQuint50", UtcDaliAlphaFunctionsEaseOutQuint50, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseOutQuint80", UtcDaliAlphaFunctionsEaseOutQuint80, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseInBack", UtcDaliAlphaFunctionsEaseInBack, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAlphaFunctionsEaseInOutBack", UtcDaliAlphaFunctionsEaseInOutBack, utc_dali_alpha_functions_startup, utc_dali_alpha_functions_cleanup},
    {"UtcDaliAngleAxisNew01", UtcDaliAngleAxisNew01, utc_dali_angle_axis_startup, utc_dali_angle_axis_cleanup},
    {"UtcDaliAngleAxisNew02", UtcDaliAngleAxisNew02, utc_dali_angle_axis_startup, utc_dali_angle_axis_cleanup},
    {"UtcDaliAngleAxisNew03", UtcDaliAngleAxisNew03, utc_dali_angle_axis_startup, utc_dali_angle_axis_cleanup},
    {"UtcDaliAngleAxisAssign", UtcDaliAngleAxisAssign, utc_dali_angle_axis_startup, utc_dali_angle_axis_cleanup},
    {"UtcDaliAngleAxisCopy", UtcDaliAngleAxisCopy, utc_dali_angle_axis_startup, utc_dali_angle_axis_cleanup},
    {"UtcDaliAnimatableMeshConstructor01", UtcDaliAnimatableMeshConstructor01, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshConstructor02", UtcDaliAnimatableMeshConstructor02, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshConstructor03", UtcDaliAnimatableMeshConstructor03, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshNew01", UtcDaliAnimatableMeshNew01, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshNew02", UtcDaliAnimatableMeshNew02, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshNew03", UtcDaliAnimatableMeshNew03, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshNew04", UtcDaliAnimatableMeshNew04, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshNew05", UtcDaliAnimatableMeshNew05, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshNew06", UtcDaliAnimatableMeshNew06, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshDownCast01", UtcDaliAnimatableMeshDownCast01, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshGetPropertyIndex01", UtcDaliAnimatableMeshGetPropertyIndex01, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshGetPropertyIndex02", UtcDaliAnimatableMeshGetPropertyIndex02, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshGetPropertyIndex03", UtcDaliAnimatableMeshGetPropertyIndex03, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshGetPropertyIndex04", UtcDaliAnimatableMeshGetPropertyIndex04, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshOperatorArray01", UtcDaliAnimatableMeshOperatorArray01, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshOperatorArray02", UtcDaliAnimatableMeshOperatorArray02, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshAnimateVertex01", UtcDaliAnimatableMeshAnimateVertex01, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableVertexSettersAndGetters", UtcDaliAnimatableVertexSettersAndGetters, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshProperties", UtcDaliAnimatableMeshProperties, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimatableMeshExceedVertices", UtcDaliAnimatableMeshExceedVertices, utc_dali_animatable_mesh_startup, utc_dali_animatable_mesh_cleanup},
    {"UtcDaliAnimationNew01", UtcDaliAnimationNew01, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationNew02", UtcDaliAnimationNew02, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationDownCast", UtcDaliAnimationDownCast, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationSetDuration", UtcDaliAnimationSetDuration, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationGetDuration", UtcDaliAnimationGetDuration, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationSetLooping", UtcDaliAnimationSetLooping, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationIsLooping", UtcDaliAnimationIsLooping, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationSetEndAction", UtcDaliAnimationSetEndAction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationGetEndAction", UtcDaliAnimationGetEndAction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationGetDestroyAction", UtcDaliAnimationGetDestroyAction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationSetDefaultAlphaFunction", UtcDaliAnimationSetDefaultAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationGetDefaultAlphaFunction", UtcDaliAnimationGetDefaultAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationPlay", UtcDaliAnimationPlay, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationPlayOffStage", UtcDaliAnimationPlayOffStage, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationPlayDiscardHandle", UtcDaliAnimationPlayDiscardHandle, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationPlayStopDiscardHandle", UtcDaliAnimationPlayStopDiscardHandle, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationPause", UtcDaliAnimationPause, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationStop", UtcDaliAnimationStop, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationStopSetPosition", UtcDaliAnimationStopSetPosition, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationClear", UtcDaliAnimationClear, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationSignalFinish", UtcDaliAnimationSignalFinish, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByBoolean", UtcDaliAnimationAnimateByBoolean, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByBooleanAlphaFunction", UtcDaliAnimationAnimateByBooleanAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByBooleanTimePeriod", UtcDaliAnimationAnimateByBooleanTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByBooleanAlphaFunctionTimePeriod", UtcDaliAnimationAnimateByBooleanAlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByFloat", UtcDaliAnimationAnimateByFloat, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByFloatAlphaFunction", UtcDaliAnimationAnimateByFloatAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByFloatTimePeriod", UtcDaliAnimationAnimateByFloatTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByFloatAlphaFunctionTimePeriod", UtcDaliAnimationAnimateByFloatAlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector2", UtcDaliAnimationAnimateByVector2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector2AlphaFunction", UtcDaliAnimationAnimateByVector2AlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector2TimePeriod", UtcDaliAnimationAnimateByVector2TimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector2AlphaFunctionTimePeriod", UtcDaliAnimationAnimateByVector2AlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector3", UtcDaliAnimationAnimateByVector3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector3AlphaFunction", UtcDaliAnimationAnimateByVector3AlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector3TimePeriod", UtcDaliAnimationAnimateByVector3TimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector3AlphaFunctionTimePeriod", UtcDaliAnimationAnimateByVector3AlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector4", UtcDaliAnimationAnimateByVector4, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector4AlphaFunction", UtcDaliAnimationAnimateByVector4AlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector4TimePeriod", UtcDaliAnimationAnimateByVector4TimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByVector4AlphaFunctionTimePeriod", UtcDaliAnimationAnimateByVector4AlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByActorPosition", UtcDaliAnimationAnimateByActorPosition, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByActorPositionAlphaFunction", UtcDaliAnimationAnimateByActorPositionAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByActorPositionTimePeriod", UtcDaliAnimationAnimateByActorPositionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateByActorPositionAlphaFunctionTimePeriod", UtcDaliAnimationAnimateByActorPositionAlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToBoolean", UtcDaliAnimationAnimateToBoolean, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToBooleanAlphaFunction", UtcDaliAnimationAnimateToBooleanAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToBooleanTimePeriod", UtcDaliAnimationAnimateToBooleanTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToBooleanAlphaFunctionTimePeriod", UtcDaliAnimationAnimateToBooleanAlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToFloat", UtcDaliAnimationAnimateToFloat, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToFloatAlphaFunction", UtcDaliAnimationAnimateToFloatAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToFloatTimePeriod", UtcDaliAnimationAnimateToFloatTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToFloatAlphaFunctionTimePeriod", UtcDaliAnimationAnimateToFloatAlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector2", UtcDaliAnimationAnimateToVector2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector2AlphaFunction", UtcDaliAnimationAnimateToVector2AlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector2TimePeriod", UtcDaliAnimationAnimateToVector2TimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector2AlphaFunctionTimePeriod", UtcDaliAnimationAnimateToVector2AlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector3", UtcDaliAnimationAnimateToVector3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector3AlphaFunction", UtcDaliAnimationAnimateToVector3AlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector3TimePeriod", UtcDaliAnimationAnimateToVector3TimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector3AlphaFunctionTimePeriod", UtcDaliAnimationAnimateToVector3AlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector3Component", UtcDaliAnimationAnimateToVector3Component, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector4", UtcDaliAnimationAnimateToVector4, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector4AlphaFunction", UtcDaliAnimationAnimateToVector4AlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector4TimePeriod", UtcDaliAnimationAnimateToVector4TimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToVector4AlphaFunctionTimePeriod", UtcDaliAnimationAnimateToVector4AlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorParentOrigin", UtcDaliAnimationAnimateToActorParentOrigin, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorParentOriginX", UtcDaliAnimationAnimateToActorParentOriginX, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorParentOriginY", UtcDaliAnimationAnimateToActorParentOriginY, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorParentOriginZ", UtcDaliAnimationAnimateToActorParentOriginZ, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorAnchorPoint", UtcDaliAnimationAnimateToActorAnchorPoint, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorAnchorPointX", UtcDaliAnimationAnimateToActorAnchorPointX, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorAnchorPointY", UtcDaliAnimationAnimateToActorAnchorPointY, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorAnchorPointZ", UtcDaliAnimationAnimateToActorAnchorPointZ, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorSize", UtcDaliAnimationAnimateToActorSize, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorSizeWidth", UtcDaliAnimationAnimateToActorSizeWidth, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorSizeHeight", UtcDaliAnimationAnimateToActorSizeHeight, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorSizeDepth", UtcDaliAnimationAnimateToActorSizeDepth, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorPosition", UtcDaliAnimationAnimateToActorPosition, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorPositionX", UtcDaliAnimationAnimateToActorPositionX, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorPositionY", UtcDaliAnimationAnimateToActorPositionY, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorPositionZ", UtcDaliAnimationAnimateToActorPositionZ, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorPositionAlphaFunction", UtcDaliAnimationAnimateToActorPositionAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorPositionTimePeriod", UtcDaliAnimationAnimateToActorPositionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorPositionAlphaFunctionTimePeriod", UtcDaliAnimationAnimateToActorPositionAlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorRotationAngleAxis", UtcDaliAnimationAnimateToActorRotationAngleAxis, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorRotationQuaternion", UtcDaliAnimationAnimateToActorRotationQuaternion, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorRotationAlphaFunction", UtcDaliAnimationAnimateToActorRotationAlphaFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorRotationTimePeriod", UtcDaliAnimationAnimateToActorRotationTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorRotationAlphaFunctionTimePeriod", UtcDaliAnimationAnimateToActorRotationAlphaFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorScale", UtcDaliAnimationAnimateToActorScale, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorScaleX", UtcDaliAnimationAnimateToActorScaleX, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorScaleY", UtcDaliAnimationAnimateToActorScaleY, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorScaleZ", UtcDaliAnimationAnimateToActorScaleZ, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorColor", UtcDaliAnimationAnimateToActorColor, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorColorRed", UtcDaliAnimationAnimateToActorColorRed, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorColorGreen", UtcDaliAnimationAnimateToActorColorGreen, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorColorBlue", UtcDaliAnimationAnimateToActorColorBlue, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateToActorColorAlpha", UtcDaliAnimationAnimateToActorColorAlpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationKeyFrames01", UtcDaliAnimationKeyFrames01, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationKeyFrames02", UtcDaliAnimationKeyFrames02, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationKeyFrames03", UtcDaliAnimationKeyFrames03, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationKeyFrames04", UtcDaliAnimationKeyFrames04, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationKeyFrames05", UtcDaliAnimationKeyFrames05, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationKeyFrames06", UtcDaliAnimationKeyFrames06, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateBetweenActorColorAlpha", UtcDaliAnimationAnimateBetweenActorColorAlpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateBetweenActorColor", UtcDaliAnimationAnimateBetweenActorColor, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateBetweenActorVisible01", UtcDaliAnimationAnimateBetweenActorVisible01, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateBetweenActorRotation02", UtcDaliAnimationAnimateBetweenActorRotation02, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationMoveByFloat3", UtcDaliAnimationMoveByFloat3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationMoveByVector3Alpha", UtcDaliAnimationMoveByVector3Alpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationMoveByVector3AlphaFloat2", UtcDaliAnimationMoveByVector3AlphaFloat2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationMoveToFloat3", UtcDaliAnimationMoveToFloat3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationMoveToVector3Alpha", UtcDaliAnimationMoveToVector3Alpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationMoveToVector3AlphaFloat2", UtcDaliAnimationMoveToVector3AlphaFloat2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationMove", UtcDaliAnimationMove, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateByDegreeVector3", UtcDaliAnimationRotateByDegreeVector3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateByRadianVector3", UtcDaliAnimationRotateByRadianVector3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateByDegreeVector3Alpha", UtcDaliAnimationRotateByDegreeVector3Alpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateByRadianVector3Alpha", UtcDaliAnimationRotateByRadianVector3Alpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateByDegreeVector3AlphaFloat2", UtcDaliAnimationRotateByDegreeVector3AlphaFloat2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateByRadianVector3AlphaFloat2", UtcDaliAnimationRotateByRadianVector3AlphaFloat2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToDegreeVector3", UtcDaliAnimationRotateToDegreeVector3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToRadianVector3", UtcDaliAnimationRotateToRadianVector3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToQuaternion", UtcDaliAnimationRotateToQuaternion, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToDegreeVector3Alpha", UtcDaliAnimationRotateToDegreeVector3Alpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToRadianVector3Alpha", UtcDaliAnimationRotateToRadianVector3Alpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToQuaternionAlpha", UtcDaliAnimationRotateToQuaternionAlpha, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToDegreeVector3AlphaFloat2", UtcDaliAnimationRotateToDegreeVector3AlphaFloat2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToRadianVector3AlphaFloat2", UtcDaliAnimationRotateToRadianVector3AlphaFloat2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotateToQuaternionAlphaFloat2", UtcDaliAnimationRotateToQuaternionAlphaFloat2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationRotate", UtcDaliAnimationRotate, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationScaleBy", UtcDaliAnimationScaleBy, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationScaleTo", UtcDaliAnimationScaleTo, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationShow", UtcDaliAnimationShow, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationHide", UtcDaliAnimationHide, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationShowHideAtEnd", UtcDaliAnimationShowHideAtEnd, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationOpacityBy", UtcDaliAnimationOpacityBy, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationOpacityTo", UtcDaliAnimationOpacityTo, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationColorBy", UtcDaliAnimationColorBy, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationColorTo", UtcDaliAnimationColorTo, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationResize", UtcDaliAnimationResize, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateBool", UtcDaliAnimationAnimateBool, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateFloat", UtcDaliAnimationAnimateFloat, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateVector2", UtcDaliAnimationAnimateVector2, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateVector3", UtcDaliAnimationAnimateVector3, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateVector4", UtcDaliAnimationAnimateVector4, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateQuaternion", UtcDaliAnimationAnimateQuaternion, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliKeyFramesCreateDestroy", UtcDaliKeyFramesCreateDestroy, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliKeyFramesDownCast", UtcDaliKeyFramesDownCast, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationResizeByXY", UtcDaliAnimationResizeByXY, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateBetweenActorColorTimePeriod", UtcDaliAnimationAnimateBetweenActorColorTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateBetweenActorColorFunction", UtcDaliAnimationAnimateBetweenActorColorFunction, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateBetweenActorColorFunctionTimePeriod", UtcDaliAnimationAnimateBetweenActorColorFunctionTimePeriod, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationAnimateVector3Func", UtcDaliAnimationAnimateVector3Func, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnimationCreateDestroy", UtcDaliAnimationCreateDestroy, utc_dali_animation_startup, utc_dali_animation_cleanup},
    {"UtcDaliAnyConstructors", UtcDaliAnyConstructors, utc_dali_any_startup, utc_dali_any_cleanup},
    {"UtcDaliAnyAssignmentOperators", UtcDaliAnyAssignmentOperators, utc_dali_any_startup, utc_dali_any_cleanup},
    {"UtcDaliAnyNegativeAssignmentOperators", UtcDaliAnyNegativeAssignmentOperators, utc_dali_any_startup, utc_dali_any_cleanup},
    {"UtcDaliAnyGetType", UtcDaliAnyGetType, utc_dali_any_startup, utc_dali_any_cleanup},
    {"UtcDaliAnyGet", UtcDaliAnyGet, utc_dali_any_startup, utc_dali_any_cleanup},
    {"UtcDaliAnyNegativeGet", UtcDaliAnyNegativeGet, utc_dali_any_startup, utc_dali_any_cleanup},
    {"UtcDaliBaseHandleConstructorVoid", UtcDaliBaseHandleConstructorVoid, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleCopyConstructor", UtcDaliBaseHandleCopyConstructor, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleAssignmentOperator", UtcDaliBaseHandleAssignmentOperator, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleGetBaseObject", UtcDaliBaseHandleGetBaseObject, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleReset", UtcDaliBaseHandleReset, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleEqualityOperator01", UtcDaliBaseHandleEqualityOperator01, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleEqualityOperator02", UtcDaliBaseHandleEqualityOperator02, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleInequalityOperator01", UtcDaliBaseHandleInequalityOperator01, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleInequalityOperator02", UtcDaliBaseHandleInequalityOperator02, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleStlVector", UtcDaliBaseHandleStlVector, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleDoAction", UtcDaliBaseHandleDoAction, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleConnectSignal", UtcDaliBaseHandleConnectSignal, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleGetTypeName", UtcDaliBaseHandleGetTypeName, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleGetObjectPtr", UtcDaliBaseHandleGetObjectPtr, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBaseHandleBooleanCast", UtcDaliBaseHandleBooleanCast, utc_base_handle_startup, utc_base_handle_cleanup},
    {"UtcDaliBitmapImageNew01", UtcDaliBitmapImageNew01, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageNew02", UtcDaliBitmapImageNew02, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageNewWithPolicy01", UtcDaliBitmapImageNewWithPolicy01, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageNewWithPolicy02", UtcDaliBitmapImageNewWithPolicy02, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageDownCast", UtcDaliBitmapImageDownCast, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageDownCast2", UtcDaliBitmapImageDownCast2, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageWHITE", UtcDaliBitmapImageWHITE, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageGetBuffer", UtcDaliBitmapImageGetBuffer, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageGetBufferSize", UtcDaliBitmapImageGetBufferSize, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageGetBufferStride", UtcDaliBitmapImageGetBufferStride, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageIsDataExternal", UtcDaliBitmapImageIsDataExternal, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageUpdate01", UtcDaliBitmapImageUpdate01, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageUpdate02", UtcDaliBitmapImageUpdate02, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageUploadedSignal01", UtcDaliBitmapImageUploadedSignal01, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliBitmapImageUploadedSignal02", UtcDaliBitmapImageUploadedSignal02, utc_dali_bitmap_image_startup, utc_dali_bitmap_image_cleanup},
    {"UtcDaliCameraActorConstructorVoid", UtcDaliCameraActorConstructorVoid, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorNew", UtcDaliCameraActorNew, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorDownCast", UtcDaliCameraActorDownCast, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorDownCast2", UtcDaliCameraActorDownCast2, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetCameraOffStage", UtcDaliCameraActorSetCameraOffStage, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetCameraOnStage", UtcDaliCameraActorSetCameraOnStage, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorGetCamera", UtcDaliCameraActorGetCamera, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorDefaultProperties", UtcDaliCameraActorDefaultProperties, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetTarget", UtcDaliCameraActorSetTarget, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetType01", UtcDaliCameraActorSetType01, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetType02", UtcDaliCameraActorSetType02, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetFieldOfView", UtcDaliCameraActorSetFieldOfView, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetAspectRatio", UtcDaliCameraActorSetAspectRatio, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetNearClippingPlane", UtcDaliCameraActorSetNearClippingPlane, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetFarClippingPlane", UtcDaliCameraActorSetFarClippingPlane, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetTargetPosition", UtcDaliCameraActorSetTargetPosition, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetInvertYAxis", UtcDaliCameraActorSetInvertYAxis, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorModelView", UtcDaliCameraActorModelView, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetPerspectiveProjection", UtcDaliCameraActorSetPerspectiveProjection, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetOrthographicProjection01", UtcDaliCameraActorSetOrthographicProjection01, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetOrthographicProjection02", UtcDaliCameraActorSetOrthographicProjection02, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorSetOrthographicProjection03", UtcDaliCameraActorSetOrthographicProjection03, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorReadProjectionMatrix", UtcDaliCameraActorReadProjectionMatrix, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorAnimatedProperties", UtcDaliCameraActorAnimatedProperties, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorPropertyIndices", UtcDaliCameraActorPropertyIndices, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorCheckLookAtAndFreeLookViews01", UtcDaliCameraActorCheckLookAtAndFreeLookViews01, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorCheckLookAtAndFreeLookViews02", UtcDaliCameraActorCheckLookAtAndFreeLookViews02, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCameraActorCheckLookAtAndFreeLookViews03", UtcDaliCameraActorCheckLookAtAndFreeLookViews03, camera_actor_test_startup, camera_actor_test_cleanup},
    {"UtcDaliCharacterCopyConstructor", UtcDaliCharacterCopyConstructor, utc_dali_character_startup, utc_dali_character_cleanup},
    {"UtcDaliCharacterComparissonOperators", UtcDaliCharacterComparissonOperators, utc_dali_character_startup, utc_dali_character_cleanup},
    {"UtcDaliCharacterAssignmentOperator", UtcDaliCharacterAssignmentOperator, utc_dali_character_startup, utc_dali_character_cleanup},
    {"UtcDaliCharacterIsLeftToRight", UtcDaliCharacterIsLeftToRight, utc_dali_character_startup, utc_dali_character_cleanup},
    {"UtcDaliGetCharacterDirection", UtcDaliGetCharacterDirection, utc_dali_character_startup, utc_dali_character_cleanup},
    {"UtcDaliCharacterIsWhiteSpace", UtcDaliCharacterIsWhiteSpace, utc_dali_character_startup, utc_dali_character_cleanup},
    {"UtcDaliCharacterIsNewLine", UtcDaliCharacterIsNewLine, utc_dali_character_startup, utc_dali_character_cleanup},
    {"UtcDaliConstraintNewBoolean", UtcDaliConstraintNewBoolean, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewFloat", UtcDaliConstraintNewFloat, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewVector2", UtcDaliConstraintNewVector2, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewVector3", UtcDaliConstraintNewVector3, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewVector4", UtcDaliConstraintNewVector4, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewMatrix", UtcDaliConstraintNewMatrix, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewMatrix3", UtcDaliConstraintNewMatrix3, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewQuaternion", UtcDaliConstraintNewQuaternion, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewOffStageBoolean", UtcDaliConstraintNewOffStageBoolean, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewOffStageFloat", UtcDaliConstraintNewOffStageFloat, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewOffStageVector2", UtcDaliConstraintNewOffStageVector2, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewOffStageVector3", UtcDaliConstraintNewOffStageVector3, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewOffStageVector4", UtcDaliConstraintNewOffStageVector4, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewOffStageQuaternion", UtcDaliConstraintNewOffStageQuaternion, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewLocalInput", UtcDaliConstraintNewLocalInput, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewParentInput", UtcDaliConstraintNewParentInput, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewInput1", UtcDaliConstraintNewInput1, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewInput2", UtcDaliConstraintNewInput2, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewInput3", UtcDaliConstraintNewInput3, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewInput4", UtcDaliConstraintNewInput4, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewInput5", UtcDaliConstraintNewInput5, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintNewInput6", UtcDaliConstraintNewInput6, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintDownCast", UtcDaliConstraintDownCast, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintSetApplyTime", UtcDaliConstraintSetApplyTime, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintGetApplyTime", UtcDaliConstraintGetApplyTime, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintSetRemoveTime", UtcDaliConstraintSetRemoveTime, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintGetRemoveTime", UtcDaliConstraintGetRemoveTime, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintSetAlphaFunction", UtcDaliConstraintSetAlphaFunction, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintGetAlphaFunction", UtcDaliConstraintGetAlphaFunction, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintSetRemoveAction", UtcDaliConstraintSetRemoveAction, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintGetRemoveAction", UtcDaliConstraintGetRemoveAction, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintRemoveDuringApply", UtcDaliConstraintRemoveDuringApply, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintImmediateRemoveDuringApply", UtcDaliConstraintImmediateRemoveDuringApply, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintActorSize", UtcDaliConstraintActorSize, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintActorSizeWidth", UtcDaliConstraintActorSizeWidth, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintActorSizeHeight", UtcDaliConstraintActorSizeHeight, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintActorSizeDepth", UtcDaliConstraintActorSizeDepth, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintInputWorldPosition", UtcDaliConstraintInputWorldPosition, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintInputWorldRotation", UtcDaliConstraintInputWorldRotation, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintInputWorldScale", UtcDaliConstraintInputWorldScale, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintInputWorldColor", UtcDaliConstraintInputWorldColor, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliConstraintInvalidInputProperty", UtcDaliConstraintInvalidInputProperty, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintParentSize", UtcDaliBuiltinConstraintParentSize, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintParentSizeRelative", UtcDaliBuiltinConstraintParentSizeRelative, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintScaleToFitConstraint", UtcDaliBuiltinConstraintScaleToFitConstraint, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintScaleToFitKeepAspectRatio", UtcDaliBuiltinConstraintScaleToFitKeepAspectRatio, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintScaleToFillKeepAspectRatio", UtcDaliBuiltinConstraintScaleToFillKeepAspectRatio, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintScaleToFillXYKeepAspectRatio", UtcDaliBuiltinConstraintScaleToFillXYKeepAspectRatio, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintShrinkInsideKeepAspectRatioConstraint", UtcDaliBuiltinConstraintShrinkInsideKeepAspectRatioConstraint, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintMultiplyConstraint", UtcDaliBuiltinConstraintMultiplyConstraint, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintDivideConstraint", UtcDaliBuiltinConstraintDivideConstraint, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintEqualToConstraint", UtcDaliBuiltinConstraintEqualToConstraint, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintRelativeToConstraint", UtcDaliBuiltinConstraintRelativeToConstraint, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintInverseOfConstraint", UtcDaliBuiltinConstraintInverseOfConstraint, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliBuiltinConstraintFunctions", UtcDaliBuiltinConstraintFunctions, utc_dali_constraint_startup, utc_dali_constraint_cleanup},
    {"UtcDaliCustomActorDestructor", UtcDaliCustomActorDestructor, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorImplDestructor", UtcDaliCustomActorImplDestructor, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorDownCast", UtcDaliCustomActorDownCast, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorDownCastNegative", UtcDaliCustomActorDownCastNegative, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnStageConnectionDisconnection", UtcDaliCustomActorOnStageConnectionDisconnection, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnStageConnectionOrder", UtcDaliCustomActorOnStageConnectionOrder, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnStageDisconnectionOrder", UtcDaliCustomActorOnStageDisconnectionOrder, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorAddDuringOnStageConnection", UtcDaliCustomActorAddDuringOnStageConnection, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorRemoveDuringOnStageConnection", UtcDaliCustomActorRemoveDuringOnStageConnection, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorAddDuringOnStageDisconnection", UtcDaliCustomActorAddDuringOnStageDisconnection, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorRemoveDuringOnStageDisconnection", UtcDaliCustomActorRemoveDuringOnStageDisconnection, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorRemoveParentDuringOnStageConnection", UtcDaliCustomActorRemoveParentDuringOnStageConnection, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorAddParentDuringOnStageDisconnection", UtcDaliCustomActorAddParentDuringOnStageDisconnection, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnChildAddRemove", UtcDaliCustomActorOnChildAddRemove, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorReparentDuringOnChildAdd", UtcDaliCustomActorReparentDuringOnChildAdd, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorRemoveDuringOnChildRemove", UtcDaliCustomActorRemoveDuringOnChildRemove, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnPropertySet", UtcDaliCustomActorOnPropertySet, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnSizeSet", UtcDaliCustomActorOnSizeSet, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnSizeAnimation", UtcDaliCustomActorOnSizeAnimation, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnTouchEvent", UtcDaliCustomActorOnTouchEvent, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorOnMouseWheelEvent", UtcDaliCustomActorOnMouseWheelEvent, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorFindChildByAlias", UtcDaliCustomActorFindChildByAlias, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorImplOnPropertySet", UtcDaliCustomActorImplOnPropertySet, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliCustomActorGetImplementation", UtcDaliCustomActorGetImplementation, custom_actor_test_startup, custom_actor_test_cleanup},
    {"UtcDaliDegreeConstructors01", UtcDaliDegreeConstructors01, utc_dali_degree_startup, utc_dali_degree_cleanup},
    {"UtcDaliDegreeComparison01", UtcDaliDegreeComparison01, utc_dali_degree_startup, utc_dali_degree_cleanup},
    {"UtcDaliDegreeCastOperators01", UtcDaliDegreeCastOperators01, utc_dali_degree_startup, utc_dali_degree_cleanup},
    {"UtcDaliDegreeCastOperatorEquals", UtcDaliDegreeCastOperatorEquals, utc_dali_degree_startup, utc_dali_degree_cleanup},
    {"UtcDaliDegreeCastOperatorNotEquals", UtcDaliDegreeCastOperatorNotEquals, utc_dali_degree_startup, utc_dali_degree_cleanup},
    {"UtcDaliDegreeCastOperatorLessThan", UtcDaliDegreeCastOperatorLessThan, utc_dali_degree_startup, utc_dali_degree_cleanup},
    {"UtcDaliInvalidEvent", UtcDaliInvalidEvent, utc_dali_event_processing_startup, utc_dali_event_processing_cleanup},
    {"UtcDaliInvalidGesture", UtcDaliInvalidGesture, utc_dali_event_processing_startup, utc_dali_event_processing_cleanup},
    {"UtcDaliFontNew01", UtcDaliFontNew01, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontNew02", UtcDaliFontNew02, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontNew03", UtcDaliFontNew03, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontNew04", UtcDaliFontNew04, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontNew05", UtcDaliFontNew05, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontNew06", UtcDaliFontNew06, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontDownCast", UtcDaliFontDownCast, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontGetPixelSize", UtcDaliFontGetPixelSize, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontGetPointSize", UtcDaliFontGetPointSize, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontPointsToPixels", UtcDaliFontPointsToPixels, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontMeasureTextWidth", UtcFontMeasureTextWidth, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontMeasureTextHeight", UtcFontMeasureTextHeight, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontMeasureText", UtcFontMeasureText, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontGetFamilyForText", UtcFontGetFamilyForText, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontGetFontLineHeightFromCapsHeight", UtcFontGetFontLineHeightFromCapsHeight, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontAllGlyphsSupported", UtcFontAllGlyphsSupported, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontGetMetrics", UtcFontGetMetrics, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontIsDefault", UtcFontIsDefault, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontGetInstalledFonts", UtcFontGetInstalledFonts, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontMetricsDefaultConstructor", UtcFontMetricsDefaultConstructor, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontMetricsCopyConstructor", UtcFontMetricsCopyConstructor, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcFontMetricsAssignmentOperator", UtcFontMetricsAssignmentOperator, utc_dali_font_startup, utc_dali_font_cleanup},
    {"UtcDaliFontParamsDefaultConstructor", UtcDaliFontParamsDefaultConstructor, utc_dali_font_parameters_startup, utc_dali_font_parameters_cleanup},
    {"UtcDaliFontParamsPointSizeConstructor", UtcDaliFontParamsPointSizeConstructor, utc_dali_font_parameters_startup, utc_dali_font_parameters_cleanup},
    {"UtcDaliFontParamsPixelSizeConstructor", UtcDaliFontParamsPixelSizeConstructor, utc_dali_font_parameters_startup, utc_dali_font_parameters_cleanup},
    {"UtcDaliFontParamsCopyConstructor", UtcDaliFontParamsCopyConstructor, utc_dali_font_parameters_startup, utc_dali_font_parameters_cleanup},
    {"UtcDaliFontParamsAssignmentOperator", UtcDaliFontParamsAssignmentOperator, utc_dali_font_parameters_startup, utc_dali_font_parameters_cleanup},
    {"UtcDaliFontParamsPointSizeEqualityOperator", UtcDaliFontParamsPointSizeEqualityOperator, utc_dali_font_parameters_startup, utc_dali_font_parameters_cleanup},
    {"UtcDaliFrameBufferImageNew01", UtcDaliFrameBufferImageNew01, utc_dali_framebuffer_startup, utc_dali_framebuffer_cleanup},
    {"UtcDaliFrameBufferImageDownCast", UtcDaliFrameBufferImageDownCast, utc_dali_framebuffer_startup, utc_dali_framebuffer_cleanup},
    {"UtcDaliGestureConstructor", UtcDaliGestureConstructor, utc_dali_gesture_startup, utc_dali_gesture_cleanup},
    {"UtcDaliGestureAssignment", UtcDaliGestureAssignment, utc_dali_gesture_startup, utc_dali_gesture_cleanup},
    {"UtcDaliGestureDetectorConstructorNegative", UtcDaliGestureDetectorConstructorNegative, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorConstructorPositive", UtcDaliGestureDetectorConstructorPositive, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorDownCast", UtcDaliGestureDetectorDownCast, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorAttachPositive", UtcDaliGestureDetectorAttachPositive, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorAttachNegative", UtcDaliGestureDetectorAttachNegative, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorDetachPositive", UtcDaliGestureDetectorDetachPositive, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorDetachNegative01", UtcDaliGestureDetectorDetachNegative01, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorDetachNegative02", UtcDaliGestureDetectorDetachNegative02, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorDetachNegative03", UtcDaliGestureDetectorDetachNegative03, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorDetachAll", UtcDaliGestureDetectorDetachAll, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorDetachAllNegative", UtcDaliGestureDetectorDetachAllNegative, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliGestureDetectorGetAttachedActors", UtcDaliGestureDetectorGetAttachedActors, utc_dali_gesture_detector_startup, utc_dali_gesture_detector_cleanup},
    {"UtcDaliHandleConstructorVoid", UtcDaliHandleConstructorVoid, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleCopyConstructor", UtcDaliHandleCopyConstructor, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleAssignmentOperator", UtcDaliHandleAssignmentOperator, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleSupports", UtcDaliHandleSupports, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleGetPropertyCount", UtcDaliHandleGetPropertyCount, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleGetPropertyName", UtcDaliHandleGetPropertyName, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleGetPropertyIndex", UtcDaliHandleGetPropertyIndex, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleIsPropertyWritable", UtcDaliHandleIsPropertyWritable, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleIsPropertyAnimatable", UtcDaliHandleIsPropertyAnimatable, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleGetPropertyType", UtcDaliHandleGetPropertyType, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleNonAnimtableProperties", UtcDaliHandleNonAnimtableProperties, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleNonAnimtableCompositeProperties", UtcDaliHandleNonAnimtableCompositeProperties, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleSetProperty01", UtcDaliHandleSetProperty01, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleSetProperty02", UtcDaliHandleSetProperty02, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleRegisterProperty", UtcDaliHandleRegisterProperty, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleGetProperty", UtcDaliHandleGetProperty, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleDownCast", UtcDaliHandleDownCast, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleCreateProperty", UtcDaliHandleCreateProperty, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleGetPropertyGet", UtcDaliHandleGetPropertyGet, handle_test_startup, handle_test_cleanup},
    {"UtcDaliHandleGetPropertyIndices", UtcDaliHandleGetPropertyIndices, handle_test_startup, handle_test_cleanup},
    {"UtcDaliImageNew01", UtcDaliImageNew01, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNew02", UtcDaliImageNew02, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNew03", UtcDaliImageNew03, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewWithPolicies01", UtcDaliImageNewWithPolicies01, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewWithPolicies02", UtcDaliImageNewWithPolicies02, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewWithPolicies03", UtcDaliImageNewWithPolicies03, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewWithPolicies04", UtcDaliImageNewWithPolicies04, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewDistanceField", UtcDaliImageNewDistanceField, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewDistanceFieldWithPolicies01", UtcDaliImageNewDistanceFieldWithPolicies01, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewDistanceFieldWithPolicies02", UtcDaliImageNewDistanceFieldWithPolicies02, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewDistanceFieldWithPolicies03", UtcDaliImageNewDistanceFieldWithPolicies03, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewDistanceFieldWithPolicies04", UtcDaliImageNewDistanceFieldWithPolicies04, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewDistanceFieldWithAttributes", UtcDaliImageNewDistanceFieldWithAttributes, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageNewDistanceFieldWithAttrandPol", UtcDaliImageNewDistanceFieldWithAttrandPol, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageDownCast", UtcDaliImageDownCast, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageGetImageSize", UtcDaliImageGetImageSize, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageGetFilename", UtcDaliImageGetFilename, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageGetLoadingState01", UtcDaliImageGetLoadingState01, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageGetLoadingState02", UtcDaliImageGetLoadingState02, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageGetReleasePolicy", UtcDaliImageGetReleasePolicy, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageGetLoadPolicy", UtcDaliImageGetLoadPolicy, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageSignalLoadingFinished", UtcDaliImageSignalLoadingFinished, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageSignalUploaded", UtcDaliImageSignalUploaded, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageDiscard01", UtcDaliImageDiscard01, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageDiscard02", UtcDaliImageDiscard02, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageDiscard03", UtcDaliImageDiscard03, utc_dali_image_startup, utc_dali_image_cleanup},
    {"UtcDaliImageActorConstructorVoid", UtcDaliImageActorConstructorVoid, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorDestructor", UtcDaliImageActorDestructor, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorNew01", UtcDaliImageActorNew01, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorNew02", UtcDaliImageActorNew02, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorDownCast", UtcDaliImageActorDownCast, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorDownCast2", UtcDaliImageActorDownCast2, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActor9Patch", UtcDaliImageActor9Patch, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorPixelArea", UtcDaliImageActorPixelArea, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorGetCurrentImageSize01", UtcDaliImageActorGetCurrentImageSize01, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorGetCurrentImageSize02", UtcDaliImageActorGetCurrentImageSize02, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorGetCurrentImageSize03", UtcDaliImageActorGetCurrentImageSize03, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorGetCurrentImageSize04", UtcDaliImageActorGetCurrentImageSize04, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorGetCurrentImageSize05", UtcDaliImageActorGetCurrentImageSize05, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorNaturalPixelAreaSize01", UtcDaliImageActorNaturalPixelAreaSize01, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorNaturalPixelAreaSize02", UtcDaliImageActorNaturalPixelAreaSize02, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorDefaultProperties", UtcDaliImageActorDefaultProperties, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorUseImageAlpha01", UtcDaliImageActorUseImageAlpha01, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorUseImageAlpha02", UtcDaliImageActorUseImageAlpha02, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorUseImageAlpha03", UtcDaliImageActorUseImageAlpha03, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorUseImageAlpha04", UtcDaliImageActorUseImageAlpha04, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorUseImageAlpha05", UtcDaliImageActorUseImageAlpha05, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorClearPixelArea", UtcDaliImageActorClearPixelArea, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageGetStyle", UtcDaliImageGetStyle, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageSetNinePatchBorder", UtcDaliImageSetNinePatchBorder, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageSetFadeIn", UtcDaliImageSetFadeIn, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageSetFadeInDuration", UtcDaliImageSetFadeInDuration, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorNewNull", UtcDaliImageActorNewNull, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorNewNullWithArea", UtcDaliImageActorNewNullWithArea, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorSetImage", UtcDaliImageActorSetImage, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageActorPropertyIndices", UtcDaliImageActorPropertyIndices, image_actor_test_startup, image_actor_test_cleanup},
    {"UtcDaliImageAttributesConstructor", UtcDaliImageAttributesConstructor, utc_dali_image_attributes_startup, utc_dali_image_attributes_cleanup},
    {"UtcDaliImageAttributesLessThan", UtcDaliImageAttributesLessThan, utc_dali_image_attributes_startup, utc_dali_image_attributes_cleanup},
    {"UtcDaliImageAttributesEquality", UtcDaliImageAttributesEquality, utc_dali_image_attributes_startup, utc_dali_image_attributes_cleanup},
    {"UtcDaliImageAttributesInEquality", UtcDaliImageAttributesInEquality, utc_dali_image_attributes_startup, utc_dali_image_attributes_cleanup},
    {"UtcDaliKeyEventConstructor", UtcDaliKeyEventConstructor, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventIsShiftModifier", UtcDaliKeyEventIsShiftModifier, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventIsCtrlModifier", UtcDaliKeyEventIsCtrlModifier, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventIsAltModifier", UtcDaliKeyEventIsAltModifier, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventIsNotShiftModifier", UtcDaliKeyEventIsNotShiftModifier, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventIsNotCtrlModifier", UtcDaliKeyEventIsNotCtrlModifier, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventIsNotAltModifier", UtcDaliKeyEventIsNotAltModifier, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventANDModifer", UtcDaliKeyEventANDModifer, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventORModifer", UtcDaliKeyEventORModifer, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliKeyEventState", UtcDaliKeyEventState, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliIntegrationKeyEvent", UtcDaliIntegrationKeyEvent, utc_dali_key_event_startup, utc_dali_key_event_cleanup},
    {"UtcDaliLayerNew", UtcDaliLayerNew, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerDownCast", UtcDaliLayerDownCast, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerDownCast2", UtcDaliLayerDownCast2, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerGetDepth", UtcDaliLayerGetDepth, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerRaise", UtcDaliLayerRaise, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerLower", UtcDaliLayerLower, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerRaiseToTop", UtcDaliLayerRaiseToTop, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerLowerToBottom", UtcDaliLayerLowerToBottom, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerSetClipping", UtcDaliLayerSetClipping, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerIsClipping", UtcDaliLayerIsClipping, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerSetClippingBox", UtcDaliLayerSetClippingBox, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerGetClippingBox", UtcDaliLayerGetClippingBox, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerSetSortFunction", UtcDaliLayerSetSortFunction, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerRaiseAbove", UtcDaliLayerRaiseAbove, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerRaiseBelow", UtcDaliLayerRaiseBelow, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerMoveAbove", UtcDaliLayerMoveAbove, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerMoveBelow", UtcDaliLayerMoveBelow, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerDefaultProperties", UtcDaliLayerDefaultProperties, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerSetDepthTestDisabled", UtcDaliLayerSetDepthTestDisabled, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerCreateDestroy", UtcDaliLayerCreateDestroy, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLayerPropertyIndices", UtcDaliLayerPropertyIndices, layer_test_startup, layer_test_cleanup},
    {"UtcDaliLongPressGestureConstructor", UtcDaliLongPressGestureConstructor, utc_dali_long_press_gesture_startup, utc_dali_long_press_gesture_cleanup},
    {"UtcDaliLongPressGestureAssignment", UtcDaliLongPressGestureAssignment, utc_dali_long_press_gesture_startup, utc_dali_long_press_gesture_cleanup},
    {"UtcDaliLongPressGestureDetectorConstructor", UtcDaliLongPressGestureDetectorConstructor, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureDetectorNew", UtcDaliLongPressGestureDetectorNew, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureDetectorDownCast", UtcDaliLongPressGestureDetectorDownCast, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSetTouchesRequired01", UtcDaliLongPressGestureSetTouchesRequired01, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSetTouchesRequired02", UtcDaliLongPressGestureSetTouchesRequired02, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureGetMinimumTouchesRequired", UtcDaliLongPressGestureGetMinimumTouchesRequired, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureGetMaximumTouchesRequired", UtcDaliLongPressGestureGetMaximumTouchesRequired, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionNegative", UtcDaliLongPressGestureSignalReceptionNegative, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionPositive", UtcDaliLongPressGestureSignalReceptionPositive, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionDetach", UtcDaliLongPressGestureSignalReceptionDetach, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionActorDestroyedDuringLongPress", UtcDaliLongPressGestureSignalReceptionActorDestroyedDuringLongPress, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionRotatedActor", UtcDaliLongPressGestureSignalReceptionRotatedActor, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionChildHit", UtcDaliLongPressGestureSignalReceptionChildHit, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionAttachDetachMany", UtcDaliLongPressGestureSignalReceptionAttachDetachMany, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionActorBecomesUntouchable", UtcDaliLongPressGestureSignalReceptionActorBecomesUntouchable, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionMultipleGestureDetectors", UtcDaliLongPressGestureSignalReceptionMultipleGestureDetectors, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionMultipleDetectorsOnActor", UtcDaliLongPressGestureSignalReceptionMultipleDetectorsOnActor, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSignalReceptionDifferentPossible", UtcDaliLongPressGestureSignalReceptionDifferentPossible, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureEmitIncorrecteStateClear", UtcDaliLongPressGestureEmitIncorrecteStateClear, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureEmitIncorrectStateContinuing", UtcDaliLongPressGestureEmitIncorrectStateContinuing, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureRepeatedState", UtcDaliLongPressGestureRepeatedState, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGesturePossibleCancelled", UtcDaliLongPressGesturePossibleCancelled, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureDetachAfterStarted", UtcDaliLongPressGestureDetachAfterStarted, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureActorUnstaged", UtcDaliLongPressGestureActorUnstaged, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureActorStagedAndDestroyed", UtcDaliLongPressGestureActorStagedAndDestroyed, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliLongPressGestureSystemOverlay", UtcDaliLongPressGestureSystemOverlay, utc_dali_long_press_gesture_detector_startup, utc_dali_long_press_gesture_detector_cleanup},
    {"UtcDaliMaterialNew01", UtcDaliMaterialNew01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialDownCast", UtcDaliMaterialDownCast, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialSettersAndGetters", UtcDaliMaterialSettersAndGetters, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialStage01", UtcDaliMaterialStage01, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialStage01MemCheck", UtcDaliMaterialStage01MemCheck, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMaterialStage02", UtcDaliMaterialStage02, utc_dali_material_startup, utc_dali_material_cleanup},
    {"UtcDaliMathUtilsNextPowerOfTwo", UtcDaliMathUtilsNextPowerOfTwo, utc_dali_math_utils_startup, utc_dali_math_utils_cleanup},
    {"UtcDaliMathUtilsIsPowerOfTwo", UtcDaliMathUtilsIsPowerOfTwo, utc_dali_math_utils_startup, utc_dali_math_utils_cleanup},
    {"UtcDaliMathUtilsGetRangedEpsilon", UtcDaliMathUtilsGetRangedEpsilon, utc_dali_math_utils_startup, utc_dali_math_utils_cleanup},
    {"UtcDaliMathUtilsRound", UtcDaliMathUtilsRound, utc_dali_math_utils_startup, utc_dali_math_utils_cleanup},
    {"UtcDaliMathUtilsClamp", UtcDaliMathUtilsClamp, utc_dali_math_utils_startup, utc_dali_math_utils_cleanup},
    {"UtcDaliMathUtilsWrapInDomain", UtcDaliMathUtilsWrapInDomain, utc_dali_math_utils_startup, utc_dali_math_utils_cleanup},
    {"UtcDaliMathUtilsShortestDistanceInDomain", UtcDaliMathUtilsShortestDistanceInDomain, utc_dali_math_utils_startup, utc_dali_math_utils_cleanup},
    {"UtcDaliMathUtilsEquals", UtcDaliMathUtilsEquals, utc_dali_math_utils_startup, utc_dali_math_utils_cleanup},
    {"UtcDaliMatrixCtor", UtcDaliMatrixCtor, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixOrthoNormalize0", UtcDaliMatrixOrthoNormalize0, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixOrthoNormalize1", UtcDaliMatrixOrthoNormalize1, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixInvert01", UtcDaliMatrixInvert01, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixInvert02", UtcDaliMatrixInvert02, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixInvertTransform01", UtcDaliMatrixInvertTransform01, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixInvertTransform02", UtcDaliMatrixInvertTransform02, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixGetXAxis", UtcDaliMatrixGetXAxis, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixGetYAxis", UtcDaliMatrixGetYAxis, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixGetZAxis", UtcDaliMatrixGetZAxis, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixGetTranslation", UtcDaliMatrixGetTranslation, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixGetTranslation3", UtcDaliMatrixGetTranslation3, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetIdentity", UtcDaliMatrixSetIdentity, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetIdentityAndScale", UtcDaliMatrixSetIdentityAndScale, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetXAxis", UtcDaliMatrixSetXAxis, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetYAxis", UtcDaliMatrixSetYAxis, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetZAxis", UtcDaliMatrixSetZAxis, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetTranslation", UtcDaliMatrixSetTranslation, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetTranslation3", UtcDaliMatrixSetTranslation3, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixTranspose", UtcDaliMatrixTranspose, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixOStreamOperator", UtcDaliMatrixOStreamOperator, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixMultiply", UtcDaliMatrixMultiply, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixOperatorMultiply", UtcDaliMatrixOperatorMultiply, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixOperatorMultiply02", UtcDaliMatrixOperatorMultiply02, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixOperatorEquals", UtcDaliMatrixOperatorEquals, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixOperatorNotEquals", UtcDaliMatrixOperatorNotEquals, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixGetTransformComponents01", UtcDaliMatrixGetTransformComponents01, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixGetTransformComponents02", UtcDaliMatrixGetTransformComponents02, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetTransformComponents01", UtcDaliMatrixSetTransformComponents01, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetInverseTransformComponent01", UtcDaliMatrixSetInverseTransformComponent01, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrixSetInverseTransformComponent02", UtcDaliMatrixSetInverseTransformComponent02, utc_dali_matrix_startup, utc_dali_matrix_cleanup},
    {"UtcDaliMatrix3FromMatrix", UtcDaliMatrix3FromMatrix, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3OperatorAssign01", UtcDaliMatrix3OperatorAssign01, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3OperatorAssign02", UtcDaliMatrix3OperatorAssign02, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3AsFloat", UtcDaliMatrix3AsFloat, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3Invert", UtcDaliMatrix3Invert, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3Transpose", UtcDaliMatrix3Transpose, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3SetIdentity", UtcDaliMatrix3SetIdentity, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3Scale", UtcDaliMatrix3Scale, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3Magnitude", UtcDaliMatrix3Magnitude, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3ScaleInverseTranspose", UtcDaliMatrix3ScaleInverseTranspose, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3OStreamOperator", UtcDaliMatrix3OStreamOperator, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3Multiply", UtcDaliMatrix3Multiply, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3EqualityOperator", UtcDaliMatrix3EqualityOperator, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMatrix3InequalityOperator", UtcDaliMatrix3InequalityOperator, utc_dali_matrix3_startup, utc_dali_matrix3_cleanup},
    {"UtcDaliMeshActorConstructorVoid", UtcDaliMeshActorConstructorVoid, mesh_actor_test_startup, mesh_actor_test_cleanup},
    {"UtcDaliMeshActorNew01", UtcDaliMeshActorNew01, mesh_actor_test_startup, mesh_actor_test_cleanup},
    {"UtcDaliMeshActorIndices", UtcDaliMeshActorIndices, mesh_actor_test_startup, mesh_actor_test_cleanup},
    {"UtcDaliPanGestureConstructor", UtcDaliPanGestureConstructor, utc_dali_pan_gesture_startup, utc_dali_pan_gesture_cleanup},
    {"UtcDaliPanGestureAssignment", UtcDaliPanGestureAssignment, utc_dali_pan_gesture_startup, utc_dali_pan_gesture_cleanup},
    {"UtcDaliPanGestureGetSpeed", UtcDaliPanGestureGetSpeed, utc_dali_pan_gesture_startup, utc_dali_pan_gesture_cleanup},
    {"UtcDaliPanGestureGetDistance", UtcDaliPanGestureGetDistance, utc_dali_pan_gesture_startup, utc_dali_pan_gesture_cleanup},
    {"UtcDaliPanGestureGetScreenSpeed", UtcDaliPanGestureGetScreenSpeed, utc_dali_pan_gesture_startup, utc_dali_pan_gesture_cleanup},
    {"UtcDaliPanGestureGetScreenDistance", UtcDaliPanGestureGetScreenDistance, utc_dali_pan_gesture_startup, utc_dali_pan_gesture_cleanup},
    {"UtcDaliPanGestureDetectorConstructor", UtcDaliPanGestureDetectorConstructor, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureDetectorNew", UtcDaliPanGestureDetectorNew, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureDetectorDownCast", UtcDaliPanGestureDetectorDownCast, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSetMinimumTouchesRequired", UtcDaliPanGestureSetMinimumTouchesRequired, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSetMaximumTouchesRequired", UtcDaliPanGestureSetMaximumTouchesRequired, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureGetMinimumTouchesRequired", UtcDaliPanGestureGetMinimumTouchesRequired, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureGetMaximumTouchesRequired", UtcDaliPanGestureGetMaximumTouchesRequired, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionNegative", UtcDaliPanGestureSignalReceptionNegative, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionDownMotionLeave", UtcDaliPanGestureSignalReceptionDownMotionLeave, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionDownMotionUp", UtcDaliPanGestureSignalReceptionDownMotionUp, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionCancelled", UtcDaliPanGestureSignalReceptionCancelled, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionDetach", UtcDaliPanGestureSignalReceptionDetach, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionDetachWhilePanning", UtcDaliPanGestureSignalReceptionDetachWhilePanning, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionActorDestroyedWhilePanning", UtcDaliPanGestureSignalReceptionActorDestroyedWhilePanning, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionRotatedActor", UtcDaliPanGestureSignalReceptionRotatedActor, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionChildHit", UtcDaliPanGestureSignalReceptionChildHit, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionAttachDetachMany", UtcDaliPanGestureSignalReceptionAttachDetachMany, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionActorBecomesUntouchable", UtcDaliPanGestureSignalReceptionActorBecomesUntouchable, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionMultipleGestureDetectors", UtcDaliPanGestureSignalReceptionMultipleGestureDetectors, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionMultipleDetectorsOnActor", UtcDaliPanGestureSignalReceptionMultipleDetectorsOnActor, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionMultipleStarted", UtcDaliPanGestureSignalReceptionMultipleStarted, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionEnsureCorrectSignalling", UtcDaliPanGestureSignalReceptionEnsureCorrectSignalling, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSignalReceptionDifferentPossible", UtcDaliPanGestureSignalReceptionDifferentPossible, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureEmitIncorrectState", UtcDaliPanGestureEmitIncorrectState, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureActorUnstaged", UtcDaliPanGestureActorUnstaged, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureActorStagedAndDestroyed", UtcDaliPanGestureActorStagedAndDestroyed, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSystemOverlay", UtcDaliPanGestureSystemOverlay, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureAngleHandling", UtcDaliPanGestureAngleHandling, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureAngleOutOfRange", UtcDaliPanGestureAngleOutOfRange, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureAngleProcessing", UtcDaliPanGestureAngleProcessing, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureDirectionHandling", UtcDaliPanGestureDirectionHandling, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureDirectionProcessing", UtcDaliPanGestureDirectionProcessing, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSetProperties", UtcDaliPanGestureSetProperties, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGestureSetPropertiesAlreadyPanning", UtcDaliPanGestureSetPropertiesAlreadyPanning, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPanGesturePropertyIndices", UtcDaliPanGesturePropertyIndices, utc_dali_pan_gesture_detector_startup, utc_dali_pan_gesture_detector_cleanup},
    {"UtcDaliPinchGestureConstructor", UtcDaliPinchGestureConstructor, utc_dali_pinch_gesture_startup, utc_dali_pinch_gesture_cleanup},
    {"UtcDaliPinchGestureAssignment", UtcDaliPinchGestureAssignment, utc_dali_pinch_gesture_startup, utc_dali_pinch_gesture_cleanup},
    {"UtcDaliPinchGestureDetectorConstructor", UtcDaliPinchGestureDetectorConstructor, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureDetectorNew", UtcDaliPinchGestureDetectorNew, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureDetectorDownCast", UtcDaliPinchGestureDetectorDownCast, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionNegative", UtcDaliPinchGestureSignalReceptionNegative, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionDownMotionLeave", UtcDaliPinchGestureSignalReceptionDownMotionLeave, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionDownMotionUp", UtcDaliPinchGestureSignalReceptionDownMotionUp, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionCancelled", UtcDaliPinchGestureSignalReceptionCancelled, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionDetach", UtcDaliPinchGestureSignalReceptionDetach, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionDetachWhilePinching", UtcDaliPinchGestureSignalReceptionDetachWhilePinching, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionActorDestroyedWhilePinching", UtcDaliPinchGestureSignalReceptionActorDestroyedWhilePinching, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionRotatedActor", UtcDaliPinchGestureSignalReceptionRotatedActor, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionChildHit", UtcDaliPinchGestureSignalReceptionChildHit, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionAttachDetachMany", UtcDaliPinchGestureSignalReceptionAttachDetachMany, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionActorBecomesUntouchable", UtcDaliPinchGestureSignalReceptionActorBecomesUntouchable, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionMultipleDetectorsOnActor", UtcDaliPinchGestureSignalReceptionMultipleDetectorsOnActor, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionMultipleStarted", UtcDaliPinchGestureSignalReceptionMultipleStarted, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSignalReceptionEnsureCorrectSignalling", UtcDaliPinchGestureSignalReceptionEnsureCorrectSignalling, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureEmitIncorrectStateClear", UtcDaliPinchGestureEmitIncorrectStateClear, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureEmitIncorrectStatePossible", UtcDaliPinchGestureEmitIncorrectStatePossible, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureActorUnstaged", UtcDaliPinchGestureActorUnstaged, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureActorStagedAndDestroyed", UtcDaliPinchGestureActorStagedAndDestroyed, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPinchGestureSystemOverlay", UtcDaliPinchGestureSystemOverlay, utc_dali_pinch_gesture_detector_startup, utc_dali_pinch_gesture_detector_cleanup},
    {"UtcDaliPixelHasAlpha", UtcDaliPixelHasAlpha, utc_dali_pixel_startup, utc_dali_pixel_cleanup},
    {"UtcDaliPixelGetBytesPerPixel", UtcDaliPixelGetBytesPerPixel, utc_dali_pixel_startup, utc_dali_pixel_cleanup},
    {"UtcDaliPixelGetAlphaOffsetAndMask", UtcDaliPixelGetAlphaOffsetAndMask, utc_dali_pixel_startup, utc_dali_pixel_cleanup},
    {"UtcDaliPropertyNotificationDownCast", UtcDaliPropertyNotificationDownCast, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationDownCastNegative", UtcDaliPropertyNotificationDownCastNegative, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliAddPropertyNotification", UtcDaliAddPropertyNotification, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliAddPropertyNotificationCallback", UtcDaliAddPropertyNotificationCallback, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliAddPropertyNotificationTypeProperty", UtcDaliAddPropertyNotificationTypeProperty, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationGetCondition", UtcDaliPropertyNotificationGetCondition, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationGetConditionConst", UtcDaliPropertyNotificationGetConditionConst, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationGetTarget", UtcDaliPropertyNotificationGetTarget, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationGetProperty", UtcDaliPropertyNotificationGetProperty, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationGetNotifyMode", UtcDaliPropertyNotificationGetNotifyMode, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationGreaterThan", UtcDaliPropertyNotificationGreaterThan, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationLessThan", UtcDaliPropertyNotificationLessThan, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationInside", UtcDaliPropertyNotificationInside, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationOutside", UtcDaliPropertyNotificationOutside, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationVectorComponentGreaterThan", UtcDaliPropertyNotificationVectorComponentGreaterThan, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationVectorComponentLessThan", UtcDaliPropertyNotificationVectorComponentLessThan, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationVectorComponentInside", UtcDaliPropertyNotificationVectorComponentInside, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationVectorComponentOutside", UtcDaliPropertyNotificationVectorComponentOutside, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyConditionGetArguments", UtcDaliPropertyConditionGetArguments, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyConditionGetArgumentsConst", UtcDaliPropertyConditionGetArgumentsConst, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationStep", UtcDaliPropertyNotificationStep, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliPropertyNotificationVariableStep", UtcDaliPropertyNotificationVariableStep, utc_dali_property_notification_startup, utc_dali_property_notification_cleanup},
    {"UtcDaliQuaternionCtor01", UtcDaliQuaternionCtor01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionCtor02", UtcDaliQuaternionCtor02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionCtor03", UtcDaliQuaternionCtor03, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionFromAxisAngle", UtcDaliQuaternionFromAxisAngle, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionToAxisAngle01", UtcDaliQuaternionToAxisAngle01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionToAxisAngle02", UtcDaliQuaternionToAxisAngle02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionToAxisAngle03", UtcDaliQuaternionToAxisAngle03, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionToAxisAngle04", UtcDaliQuaternionToAxisAngle04, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionEulerAngles", UtcDaliQuaternionEulerAngles, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionToMatrix01", UtcDaliQuaternionToMatrix01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionToMatrix02", UtcDaliQuaternionToMatrix02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionFromMatrix01", UtcDaliQuaternionFromMatrix01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionFromMatrix02", UtcDaliQuaternionFromMatrix02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionFromAxes01", UtcDaliQuaternionFromAxes01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionFromAxes02", UtcDaliQuaternionFromAxes02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorAddition", UtcDaliQuaternionOperatorAddition, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorSubtraction", UtcDaliQuaternionOperatorSubtraction, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionConjugate", UtcDaliQuaternionConjugate, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorMultiplication01", UtcDaliQuaternionOperatorMultiplication01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorDivision", UtcDaliQuaternionOperatorDivision, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorScale01", UtcDaliQuaternionOperatorScale01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorScale02", UtcDaliQuaternionOperatorScale02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorNegation", UtcDaliQuaternionOperatorNegation, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorAddAssign", UtcDaliQuaternionOperatorAddAssign, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorSubtractAssign", UtcDaliQuaternionOperatorSubtractAssign, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorMultiplyAssign", UtcDaliQuaternionOperatorMultiplyAssign, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorScaleAssign01", UtcDaliQuaternionOperatorScaleAssign01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorScaleAssign02", UtcDaliQuaternionOperatorScaleAssign02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorEquality", UtcDaliQuaternionOperatorEquality, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorInequality", UtcDaliQuaternionOperatorInequality, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionLength", UtcDaliQuaternionLength, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionLengthSquared", UtcDaliQuaternionLengthSquared, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionNormalize", UtcDaliQuaternionNormalize, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionNormalized", UtcDaliQuaternionNormalized, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionInvert", UtcDaliQuaternionInvert, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionDot", UtcDaliQuaternionDot, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOperatorMultiplication02", UtcDaliQuaternionOperatorMultiplication02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionRotate01", UtcDaliQuaternionRotate01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionRotate02", UtcDaliQuaternionRotate02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionExp01", UtcDaliQuaternionExp01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionExp02", UtcDaliQuaternionExp02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionExp03", UtcDaliQuaternionExp03, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionLog01", UtcDaliQuaternionLog01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionLog02", UtcDaliQuaternionLog02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionLerp", UtcDaliQuaternionLerp, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionSlerp01", UtcDaliQuaternionSlerp01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionSlerp02", UtcDaliQuaternionSlerp02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionSlerp03", UtcDaliQuaternionSlerp03, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionSlerp04", UtcDaliQuaternionSlerp04, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionSlerpNoInvert01", UtcDaliQuaternionSlerpNoInvert01, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionSlerpNoInvert02", UtcDaliQuaternionSlerpNoInvert02, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionSquad", UtcDaliQuaternionSquad, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliAngleBetween", UtcDaliAngleBetween, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliQuaternionOStreamOperator", UtcDaliQuaternionOStreamOperator, utc_dali_quaternion_startup, utc_dali_quaternion_cleanup},
    {"UtcDaliRadianConstructors01", UtcDaliRadianConstructors01, utc_dali_radian_startup, utc_dali_radian_cleanup},
    {"UtcDaliRadianComparison01", UtcDaliRadianComparison01, utc_dali_radian_startup, utc_dali_radian_cleanup},
    {"UtcDaliRadianCastOperators01", UtcDaliRadianCastOperators01, utc_dali_radian_startup, utc_dali_radian_cleanup},
    {"UtcDaliRadianCastOperatorEquals", UtcDaliRadianCastOperatorEquals, utc_dali_radian_startup, utc_dali_radian_cleanup},
    {"UtcDaliRadianCastOperatorNotEquals", UtcDaliRadianCastOperatorNotEquals, utc_dali_radian_startup, utc_dali_radian_cleanup},
    {"UtcDaliRadianCastOperatorLessThan", UtcDaliRadianCastOperatorLessThan, utc_dali_radian_startup, utc_dali_radian_cleanup},
    {"UtcDaliRectCons01", UtcDaliRectCons01, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectCons02", UtcDaliRectCons02, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectCons03", UtcDaliRectCons03, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectCons04", UtcDaliRectCons04, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectSet", UtcDaliRectSet, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectIsEmpty", UtcDaliRectIsEmpty, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectRight", UtcDaliRectRight, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectBottom", UtcDaliRectBottom, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectArea", UtcDaliRectArea, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectIntersects", UtcDaliRectIntersects, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectContains", UtcDaliRectContains, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectOperatorNotEquals", UtcDaliRectOperatorNotEquals, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRectOperatorEquals", UtcDaliRectOperatorEquals, utc_dali_rect_startup, utc_dali_rect_cleanup},
    {"UtcDaliRenderTaskDownCast", UtcDaliRenderTaskDownCast, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetSourceActor", UtcDaliRenderTaskSetSourceActor, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetSourceActorOffStage", UtcDaliRenderTaskSetSourceActorOffStage, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetSourceActorEmpty", UtcDaliRenderTaskSetSourceActorEmpty, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetSourceActor", UtcDaliRenderTaskGetSourceActor, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetExclusive", UtcDaliRenderTaskSetExclusive, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskIsExclusive", UtcDaliRenderTaskIsExclusive, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetInputEnabled", UtcDaliRenderTaskSetInputEnabled, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetInputEnabled", UtcDaliRenderTaskGetInputEnabled, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetCameraActor", UtcDaliRenderTaskSetCameraActor, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetCameraActor", UtcDaliRenderTaskGetCameraActor, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetTargetFrameBuffer", UtcDaliRenderTaskSetTargetFrameBuffer, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetTargetFrameBuffer", UtcDaliRenderTaskGetTargetFrameBuffer, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetScreenToFrameBufferFunction", UtcDaliRenderTaskSetScreenToFrameBufferFunction, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetScreenToFrameBufferFunction", UtcDaliRenderTaskGetScreenToFrameBufferFunction, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetScreenToFrameBufferMappingActor", UtcDaliRenderTaskGetScreenToFrameBufferMappingActor, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetViewport", UtcDaliRenderTaskSetViewport, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetViewport", UtcDaliRenderTaskGetViewport, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetViewportPosition", UtcDaliRenderTaskSetViewportPosition, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetViewportSize", UtcDaliRenderTaskSetViewportSize, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetClearColor", UtcDaliRenderTaskSetClearColor, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetClearColor", UtcDaliRenderTaskGetClearColor, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetClearEnabled", UtcDaliRenderTaskSetClearEnabled, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetClearEnabled", UtcDaliRenderTaskGetClearEnabled, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSetRefreshRate", UtcDaliRenderTaskSetRefreshRate, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskGetRefreshRate", UtcDaliRenderTaskGetRefreshRate, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskSignalFinished", UtcDaliRenderTaskSignalFinished, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskContinuous01", UtcDaliRenderTaskContinuous01, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskContinuous02", UtcDaliRenderTaskContinuous02, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskContinuous03", UtcDaliRenderTaskContinuous03, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskContinuous04", UtcDaliRenderTaskContinuous04, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskContinuous05", UtcDaliRenderTaskContinuous05, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskContinuous06", UtcDaliRenderTaskContinuous06, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnce01", UtcDaliRenderTaskOnce01, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnce02", UtcDaliRenderTaskOnce02, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnce03", UtcDaliRenderTaskOnce03, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnce05", UtcDaliRenderTaskOnce05, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnce06", UtcDaliRenderTaskOnce06, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnce07", UtcDaliRenderTaskOnce07, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnce08", UtcDaliRenderTaskOnce08, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnceNoSync01", UtcDaliRenderTaskOnceNoSync01, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnceNoSync02", UtcDaliRenderTaskOnceNoSync02, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnceNoSync03", UtcDaliRenderTaskOnceNoSync03, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnceNoSync05", UtcDaliRenderTaskOnceNoSync05, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnceNoSync06", UtcDaliRenderTaskOnceNoSync06, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnceNoSync07", UtcDaliRenderTaskOnceNoSync07, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnceNoSync08", UtcDaliRenderTaskOnceNoSync08, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskOnceChain01", UtcDaliRenderTaskOnceChain01, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskProperties", UtcDaliRenderTaskProperties, utc_dali_render_task_startup, utc_dali_render_task_cleanup},
    {"UtcDaliRenderTaskListDefaultConstructor", UtcDaliRenderTaskListDefaultConstructor, utc_dali_render_task_list_startup, utc_dali_render_task_list_cleanup},
    {"UtcDaliRenderTaskListDownCast", UtcDaliRenderTaskListDownCast, utc_dali_render_task_list_startup, utc_dali_render_task_list_cleanup},
    {"UtcDaliRenderTaskListCreateTask", UtcDaliRenderTaskListCreateTask, utc_dali_render_task_list_startup, utc_dali_render_task_list_cleanup},
    {"UtcDaliRenderTaskListRemoveTask", UtcDaliRenderTaskListRemoveTask, utc_dali_render_task_list_startup, utc_dali_render_task_list_cleanup},
    {"UtcDaliRenderTaskListGetTaskCount", UtcDaliRenderTaskListGetTaskCount, utc_dali_render_task_list_startup, utc_dali_render_task_list_cleanup},
    {"UtcDaliRenderTaskListGetTask", UtcDaliRenderTaskListGetTask, utc_dali_render_task_list_startup, utc_dali_render_task_list_cleanup},
    {"UtcDaliRenderableActorDownCast", UtcDaliRenderableActorDownCast, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorSetSortModifier", UtcDaliRenderableActorSetSortModifier, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorGetSortModifier", UtcDaliRenderableActorGetSortModifier, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorSetGetBlendMode", UtcDaliRenderableActorSetGetBlendMode, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorSetCullFace", UtcDaliRenderableActorSetCullFace, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorGetCullFace", UtcDaliRenderableActorGetCullFace, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorSetGetBlendFunc", UtcDaliRenderableActorSetGetBlendFunc, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorSetGetBlendEquation", UtcDaliRenderableActorSetGetBlendEquation, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorSetGetBlendColor", UtcDaliRenderableActorSetGetBlendColor, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorSetGetAlpha", UtcDaliRenderableActorSetGetAlpha, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliRenderableActorCreateDestroy", UtcDaliRenderableActorCreateDestroy, renderable_actor_startup, renderable_actor_cleanup},
    {"UtcDaliShaderEffectMethodNew01", UtcDaliShaderEffectMethodNew01, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodNew02", UtcDaliShaderEffectMethodNew02, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodNew03", UtcDaliShaderEffectMethodNew03, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodNew04", UtcDaliShaderEffectMethodNew04, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodNew05", UtcDaliShaderEffectMethodNew05, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodNew06", UtcDaliShaderEffectMethodNew06, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodDownCast", UtcDaliShaderEffectMethodDownCast, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodDelete01", UtcDaliShaderEffectMethodDelete01, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetUniformFloat", UtcDaliShaderEffectMethodSetUniformFloat, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetUniformVector2", UtcDaliShaderEffectMethodSetUniformVector2, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetUniformVector3", UtcDaliShaderEffectMethodSetUniformVector3, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetUniformVector4", UtcDaliShaderEffectMethodSetUniformVector4, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetUniformMatrix", UtcDaliShaderEffectMethodSetUniformMatrix, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetUniformMatrix3", UtcDaliShaderEffectMethodSetUniformMatrix3, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetUniformViewport", UtcDaliShaderEffectMethodSetUniformViewport, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetEffectImage", UtcDaliShaderEffectMethodSetEffectImage, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodSetEffectImageAndDelete", UtcDaliShaderEffectMethodSetEffectImageAndDelete, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodApplyConstraint", UtcDaliShaderEffectMethodApplyConstraint, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodApplyConstraintFromActor", UtcDaliShaderEffectMethodApplyConstraintFromActor, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodApplyConstraintFromActor2", UtcDaliShaderEffectMethodApplyConstraintFromActor2, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodApplyConstraintCallback", UtcDaliShaderEffectMethodApplyConstraintCallback, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodRemoveConstraints", UtcDaliShaderEffectMethodRemoveConstraints, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodRemoveConstraints2", UtcDaliShaderEffectMethodRemoveConstraints2, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodCreateExtension", UtcDaliShaderEffectMethodCreateExtension, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodCreateExtension2", UtcDaliShaderEffectMethodCreateExtension2, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectMethodNoExtension", UtcDaliShaderEffectMethodNoExtension, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliShaderEffectPropertyIndices", UtcDaliShaderEffectPropertyIndices, utc_dali_shader_effect_startup, utc_dali_shader_effect_cleanup},
    {"UtcDaliSignalFunctorsEmptyCheck", UtcDaliSignalFunctorsEmptyCheck, utc_dali_signal_templates_functors_startup, utc_dali_signal_templates_functors_cleanup},
    {"UtcDaliSignalFunctorsEmit", UtcDaliSignalFunctorsEmit, utc_dali_signal_templates_functors_startup, utc_dali_signal_templates_functors_cleanup},
    {"UtcDaliSignalFunctorsEmitReturn", UtcDaliSignalFunctorsEmitReturn, utc_dali_signal_templates_functors_startup, utc_dali_signal_templates_functors_cleanup},
    {"UtcDaliSignalFunctorsDisconnectBeforeEmit", UtcDaliSignalFunctorsDisconnectBeforeEmit, utc_dali_signal_templates_functors_startup, utc_dali_signal_templates_functors_cleanup},
    {"UtcDaliSignalFunctorsDestroySignal", UtcDaliSignalFunctorsDestroySignal, utc_dali_signal_templates_functors_startup, utc_dali_signal_templates_functors_cleanup},
    {"UtcDaliSignalConnectVoidFunctor", UtcDaliSignalConnectVoidFunctor, utc_dali_signal_templates_functors_startup, utc_dali_signal_templates_functors_cleanup},
    {"UtcDaliSignalEmptyCheck", UtcDaliSignalEmptyCheck, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalEmptyCheckSlotDestruction", UtcDaliSignalEmptyCheckSlotDestruction, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalConnectAndEmit", UtcDaliSignalConnectAndEmit, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalDisconnect", UtcDaliSignalDisconnect, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalDisconnect2", UtcDaliSignalDisconnect2, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalDisconnect3", UtcDaliSignalDisconnect3, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalCustomConnectionTracker", UtcDaliSignalCustomConnectionTracker, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalMultipleConnections", UtcDaliSignalMultipleConnections, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalMultipleConnections2", UtcDaliSignalMultipleConnections2, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalMultipleConnections3", UtcDaliSignalMultipleConnections3, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalDisconnectStatic", UtcDaliSignalDisconnectStatic, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalDisconnectDuringCallback", UtcDaliSignalDisconnectDuringCallback, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalDisconnectDuringCallback2", UtcDaliSignalDisconnectDuringCallback2, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalEmitDuringCallback", UtcDaliSignalEmitDuringCallback, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalTestApp01", UtcDaliSignalTestApp01, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalTestApp02", UtcDaliSignalTestApp02, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalTestApp03", UtcDaliSignalTestApp03, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalTestApp04", UtcDaliSignalTestApp04, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalTestApp05", UtcDaliSignalTestApp05, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalTestApp06", UtcDaliSignalTestApp06, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSlotDelegateConnection", UtcDaliSlotDelegateConnection, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSignalSlotDelegateDestruction", UtcDaliSignalSlotDelegateDestruction, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliSlotHandlerDisconnect", UtcDaliSlotHandlerDisconnect, utc_dali_signal_templates_startup, utc_dali_signal_templates_cleanup},
    {"UtcDaliStageDefaultConstructor", UtcDaliStageDefaultConstructor, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageDestructor", UtcDaliStageDestructor, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetCurrent", UtcDaliStageGetCurrent, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageIsInstalled", UtcDaliStageIsInstalled, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageAdd", UtcDaliStageAdd, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageRemove", UtcDaliStageRemove, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetSize", UtcDaliStageGetSize, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetDpi01", UtcDaliStageGetDpi01, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetDpi02", UtcDaliStageGetDpi02, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetDpi03", UtcDaliStageGetDpi03, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetLayerCount", UtcDaliStageGetLayerCount, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetLayer", UtcDaliStageGetLayer, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetRootLayer", UtcDaliStageGetRootLayer, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageSetBackgroundColor", UtcDaliStageSetBackgroundColor, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageGetBackgroundColor", UtcDaliStageGetBackgroundColor, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageKeepRendering", UtcDaliStageKeepRendering, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageEventProcessingFinished", UtcDaliStageEventProcessingFinished, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageSignalKeyEvent", UtcDaliStageSignalKeyEvent, stage_test_startup, stage_test_cleanup},
    {"UtcDaliStageTouchedSignal", UtcDaliStageTouchedSignal, stage_test_startup, stage_test_cleanup},
    {"UtcDaliTapGestureConstructor", UtcDaliTapGestureConstructor, utc_dali_tap_gesture_startup, utc_dali_tap_gesture_cleanup},
    {"UtcDaliTapGestureAssignment", UtcDaliTapGestureAssignment, utc_dali_tap_gesture_startup, utc_dali_tap_gesture_cleanup},
    {"UtcDaliTapGestureDetectorConstructor", UtcDaliTapGestureDetectorConstructor, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureDetectorNew", UtcDaliTapGestureDetectorNew, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureDetectorDownCast", UtcDaliTapGestureDetectorDownCast, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSetTapsRequired", UtcDaliTapGestureSetTapsRequired, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureGetTapsRequired", UtcDaliTapGestureGetTapsRequired, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSetTouchesRequired", UtcDaliTapGestureSetTouchesRequired, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureGetTouchesRequired", UtcDaliTapGestureGetTouchesRequired, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionNegative", UtcDaliTapGestureSignalReceptionNegative, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionPositive", UtcDaliTapGestureSignalReceptionPositive, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionDetach", UtcDaliTapGestureSignalReceptionDetach, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionActorDestroyedWhileTapping", UtcDaliTapGestureSignalReceptionActorDestroyedWhileTapping, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionRotatedActor", UtcDaliTapGestureSignalReceptionRotatedActor, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionChildHit", UtcDaliTapGestureSignalReceptionChildHit, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionAttachDetachMany", UtcDaliTapGestureSignalReceptionAttachDetachMany, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionActorBecomesUntouchable", UtcDaliTapGestureSignalReceptionActorBecomesUntouchable, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionMultipleGestureDetectors", UtcDaliTapGestureSignalReceptionMultipleGestureDetectors, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionMultipleDetectorsOnActor", UtcDaliTapGestureSignalReceptionMultipleDetectorsOnActor, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSignalReceptionDifferentPossible", UtcDaliTapGestureSignalReceptionDifferentPossible, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureEmitIncorrectStateClear", UtcDaliTapGestureEmitIncorrectStateClear, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureEmitIncorrectStateContinuing", UtcDaliTapGestureEmitIncorrectStateContinuing, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureEmitIncorrectStateFinished", UtcDaliTapGestureEmitIncorrectStateFinished, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureActorUnstaged", UtcDaliTapGestureActorUnstaged, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureRepeatedState", UtcDaliTapGestureRepeatedState, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGesturePossibleCancelled", UtcDaliTapGesturePossibleCancelled, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureDetectorRemovedWhilePossible", UtcDaliTapGestureDetectorRemovedWhilePossible, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureActorRemovedWhilePossible", UtcDaliTapGestureActorRemovedWhilePossible, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTapGestureSystemOverlay", UtcDaliTapGestureSystemOverlay, utc_dali_tap_gesture_detector_startup, utc_dali_tap_gesture_detector_cleanup},
    {"UtcDaliTextConstructor", UtcDaliTextConstructor, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextCopyConstructor", UtcDaliTextCopyConstructor, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextAssignmentOperator", UtcDaliTextAssignmentOperator, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextSetGetText", UtcDaliTextSetGetText, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextAccessOperator01", UtcDaliTextAccessOperator01, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextAccessOperator02", UtcDaliTextAccessOperator02, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextIsEmpty", UtcDaliTextIsEmpty, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextGetLength", UtcDaliTextGetLength, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextAppend", UtcDaliTextAppend, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextRemove01", UtcDaliTextRemove01, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextRemove02", UtcDaliTextRemove02, utc_dali_text_startup, utc_dali_text_cleanup},
    {"UtcDaliTextActorConstructorVoid", UtcDaliTextActorConstructorVoid, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorNew01", UtcDaliTextActorNew01, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorNew02", UtcDaliTextActorNew02, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorNew03", UtcDaliTextActorNew03, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorNew04", UtcDaliTextActorNew04, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorNew05", UtcDaliTextActorNew05, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorNew06", UtcDaliTextActorNew06, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorNew07", UtcDaliTextActorNew07, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorDownCast", UtcDaliTextActorDownCast, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorDownCast2", UtcDaliTextActorDownCast2, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetText", UtcDaliTextActorSetText, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetFont", UtcDaliTextActorSetFont, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetFontDetection", UtcDaliTextActorSetFontDetection, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetTextIndividualStyles", UtcDaliTextActorSetTextIndividualStyles, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorChangingText", UtcDaliTextActorChangingText, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorGetLoadingState", UtcDaliTextActorGetLoadingState, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetItalics", UtcDaliTextActorSetItalics, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetUnderline", UtcDaliTextActorSetUnderline, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetWeight", UtcDaliTextActorSetWeight, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetStyle", UtcDaliTextActorSetStyle, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorDefaultProperties", UtcDaliTextActorDefaultProperties, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetGradientColor", UtcDaliTextActorSetGradientColor, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetGradientStartPoint", UtcDaliTextActorSetGradientStartPoint, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSetGradientEndPoint", UtcDaliTextActorSetGradientEndPoint, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorSynchronousGlyphLoading", UtcDaliTextActorSynchronousGlyphLoading, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorAutomaticSizeSet", UtcDaliTextActorAutomaticSizeSet, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorAutomaticSizeSetAnimation", UtcDaliTextActorAutomaticSizeSetAnimation, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextActorPropertyIndices", UtcDaliTextActorPropertyIndices, text_actor_test_startup, text_actor_test_cleanup},
    {"UtcDaliTextStyleDefaultConstructor", UtcDaliTextStyleDefaultConstructor, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleCopyConstructor", UtcDaliTextStyleCopyConstructor, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleComparisonOperator", UtcDaliTextStyleComparisonOperator, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleCopy", UtcDaliTextStyleCopy, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetFontName", UtcDaliTextStyleSetGetFontName, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetFontStyle", UtcDaliTextStyleSetGetFontStyle, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetFontPointSize", UtcDaliTextStyleSetGetFontPointSize, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetWeight", UtcDaliTextStyleSetGetWeight, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetTextColor", UtcDaliTextStyleSetGetTextColor, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetItalics", UtcDaliTextStyleSetGetItalics, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetItalicsAngle", UtcDaliTextStyleSetGetItalicsAngle, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetUnderline", UtcDaliTextStyleSetGetUnderline, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetShadow", UtcDaliTextStyleSetGetShadow, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetGlow", UtcDaliTextStyleSetGetGlow, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetOutline", UtcDaliTextStyleSetGetOutline, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTextStyleSetGetSmoothEdge", UtcDaliTextStyleSetGetSmoothEdge, utc_dali_text_style_startup, utc_dali_text_style_cleanup},
    {"UtcDaliTouchEventCombinerConstructors", UtcDaliTouchEventCombinerConstructors, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerConstructorsNegative", UtcDaliTouchEventCombinerConstructorsNegative, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSettersAndGetters", UtcDaliTouchEventCombinerSettersAndGetters, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSettersNegative", UtcDaliTouchEventCombinerSettersNegative, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSingleTouchNormal", UtcDaliTouchEventCombinerSingleTouchNormal, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSingleTouchMotionWithoutDown", UtcDaliTouchEventCombinerSingleTouchMotionWithoutDown, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSingleTouchTwoDowns", UtcDaliTouchEventCombinerSingleTouchTwoDowns, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSingleTouchUpWithoutDown", UtcDaliTouchEventCombinerSingleTouchUpWithoutDown, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSingleTouchTwoUps", UtcDaliTouchEventCombinerSingleTouchTwoUps, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSingleTouchUpWithDifferentId", UtcDaliTouchEventCombinerSingleTouchUpWithDifferentId, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSingleTouchMotionWithDifferentId", UtcDaliTouchEventCombinerSingleTouchMotionWithDifferentId, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerMultiTouchNormal", UtcDaliTouchEventCombinerMultiTouchNormal, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSeveralPoints", UtcDaliTouchEventCombinerSeveralPoints, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerReset", UtcDaliTouchEventCombinerReset, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerSingleTouchInterrupted", UtcDaliTouchEventCombinerSingleTouchInterrupted, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerMultiTouchInterrupted", UtcDaliTouchEventCombinerMultiTouchInterrupted, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchEventCombinerInvalidState", UtcDaliTouchEventCombinerInvalidState, utc_dali_touch_event_combiner_startup, utc_dali_touch_event_combiner_cleanup},
    {"UtcDaliTouchNormalProcessing", UtcDaliTouchNormalProcessing, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchOutsideCameraNearFarPlanes", UtcDaliTouchOutsideCameraNearFarPlanes, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchEmitEmpty", UtcDaliTouchEmitEmpty, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchInterrupted", UtcDaliTouchInterrupted, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchParentConsumer", UtcDaliTouchParentConsumer, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchInterruptedParentConsumer", UtcDaliTouchInterruptedParentConsumer, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchLeave", UtcDaliTouchLeave, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchLeaveParentConsumer", UtcDaliTouchLeaveParentConsumer, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchActorBecomesInsensitive", UtcDaliTouchActorBecomesInsensitive, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchActorBecomesInsensitiveParentConsumer", UtcDaliTouchActorBecomesInsensitiveParentConsumer, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchMultipleLayers", UtcDaliTouchMultipleLayers, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchMultipleRenderTasks", UtcDaliTouchMultipleRenderTasks, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchMultipleRenderTasksWithChildLayer", UtcDaliTouchMultipleRenderTasksWithChildLayer, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchOffscreenRenderTasks", UtcDaliTouchOffscreenRenderTasks, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchMultipleRenderableActors", UtcDaliTouchMultipleRenderableActors, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchActorRemovedInSignal", UtcDaliTouchActorRemovedInSignal, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchActorSignalNotConsumed", UtcDaliTouchActorSignalNotConsumed, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchActorUnStaged", UtcDaliTouchActorUnStaged, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliTouchSystemOverlayActor", UtcDaliTouchSystemOverlayActor, utc_dali_touch_processing_startup, utc_dali_touch_processing_cleanup},
    {"UtcDaliVector2Cons", UtcDaliVector2Cons, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2FitInside", UtcDaliVector2FitInside, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2FitScaleToFill", UtcDaliVector2FitScaleToFill, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2ShrinkInside", UtcDaliVector2ShrinkInside, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Add", UtcDaliVector2Add, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Subtract", UtcDaliVector2Subtract, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Negate", UtcDaliVector2Negate, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Multiply", UtcDaliVector2Multiply, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Divide", UtcDaliVector2Divide, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Scale", UtcDaliVector2Scale, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Equals", UtcDaliVector2Equals, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Length", UtcDaliVector2Length, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2LengthSquared", UtcDaliVector2LengthSquared, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Max", UtcDaliVector2Max, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Min", UtcDaliVector2Min, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Clamp", UtcDaliVector2Clamp, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2ClampVector2", UtcDaliVector2ClampVector2, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2Normalize", UtcDaliVector2Normalize, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2OperatorSubscript", UtcDaliVector2OperatorSubscript, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector2OStreamOperator", UtcDaliVector2OStreamOperator, utc_dali_vector2_startup, utc_dali_vector2_cleanup},
    {"UtcDaliVector3Cons", UtcDaliVector3Cons, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Assign", UtcDaliVector3Assign, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Add", UtcDaliVector3Add, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Constants", UtcDaliVector3Constants, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Cross", UtcDaliVector3Cross, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Dot", UtcDaliVector3Dot, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Equals", UtcDaliVector3Equals, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Length", UtcDaliVector3Length, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3LengthSquared", UtcDaliVector3LengthSquared, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Max", UtcDaliVector3Max, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Min", UtcDaliVector3Min, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Clamp", UtcDaliVector3Clamp, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3ClampVector3", UtcDaliVector3ClampVector3, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Multiply", UtcDaliVector3Multiply, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Divide", UtcDaliVector3Divide, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Scale", UtcDaliVector3Scale, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Normalize", UtcDaliVector3Normalize, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Subtract", UtcDaliVector3Subtract, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3OperatorSubscript", UtcDaliVector3OperatorSubscript, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3OStreamOperator", UtcDaliVector3OStreamOperator, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3Rotate", UtcDaliVector3Rotate, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3AsFloat", UtcDaliVector3AsFloat, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3AsVectorXY", UtcDaliVector3AsVectorXY, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3FitKeepAspectRatio", UtcDaliVector3FitKeepAspectRatio, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3FillKeepAspectRatio", UtcDaliVector3FillKeepAspectRatio, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3FillXYKeepAspectRatio", UtcDaliVector3FillXYKeepAspectRatio, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector3ShrinkInsideKeepAspectRatio", UtcDaliVector3ShrinkInsideKeepAspectRatio, utc_dali_vector3_startup, utc_dali_vector3_cleanup},
    {"UtcDaliVector4Vector4", UtcDaliVector4Vector4, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Add", UtcDaliVector4Add, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Constants", UtcDaliVector4Constants, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Cross", UtcDaliVector4Cross, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Dot", UtcDaliVector4Dot, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4DotVector3", UtcDaliVector4DotVector3, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Dot4", UtcDaliVector4Dot4, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Equals", UtcDaliVector4Equals, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Length", UtcDaliVector4Length, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4LengthSquared", UtcDaliVector4LengthSquared, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Max", UtcDaliVector4Max, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Min", UtcDaliVector4Min, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Clamp", UtcDaliVector4Clamp, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4ClampVector4", UtcDaliVector4ClampVector4, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Multiply", UtcDaliVector4Multiply, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Divide", UtcDaliVector4Divide, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Scale", UtcDaliVector4Scale, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Normalize", UtcDaliVector4Normalize, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4Subtract", UtcDaliVector4Subtract, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4OperatorSubscript", UtcDaliVector4OperatorSubscript, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4OStreamOperator", UtcDaliVector4OStreamOperator, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {"UtcDaliVector4AsFloat", UtcDaliVector4AsFloat, utc_dali_vector4_startup, utc_dali_vector4_cleanup},
    {NULL, NULL}
};

#endif // __TCT_DALI_CORE_H__
