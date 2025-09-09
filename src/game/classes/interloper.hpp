#pragma once


enum eEmoteState
{
    EEmoteState_None,
    EEmoteState_Playing,
    EEmoteState_Playing_Sit,
};

enum eRocketBootsDoubleTapState
{
    None,
    WaitingForRelease,
    WaitingForSecondTap,
    SingleTap,
    DoubleTap,
};

enum eStaminaState
{
    EStamina_Available,
    EStamina_Recovering,
};

enum PlayerMode
{
    Unknown,
    InShip,
    OnSurface,
};

enum eRocketBoostState
{
    Inactive,
    WindUp,
    Boost,
    Drift,
    Landing,
};

enum eCharacterMode
{
    ECharacterMode_FirstPerson,
    ECharacterMode_ThirdPerson,
};

struct sGrabbedObjectInfo
{
    const /*cGcGrabbableComponent*/ uintptr_t* mpGrabbedComponent;
    float mfMovementCompletion;
    int miDataIndex;
    bool mbIsAttached;
    bool mbGrabToggleActive;
    float mfGrabTime;
};

struct FootstepOnDistanceTravel
{
    float mfTriggerDistance;
    float mfTriggerDistanceSqr;
    bool mbValidDistance;
    bool mbValidPosition;
};

struct cGcSummonPetData
{
    int miIndex;
    float mfAdultScale;
    float mfGrowthProgress;
    bool mbHasFur;
};

class GcPlayer
{
public:
    float mfLastGrabbedTime;
    float mfGrabEscapeTimer;
    float mfThrowFoodCooldown;
    float mfRecoilSpeed;
    float mfRecoilAmount;
    float mfDamagePerSecondRate;
    uint8_t meWarpTargetMode[4];
    bool mbHandMatrixValid[2];
    bool mbReleaseGrabbable[2];
    bool mbSwimHandPosActive[2];
    bool mbHandTrackingMotionEnabled;
    bool mbSpawned;
    bool mbLanded;
    bool mbRunning;
    float mfHardLandTimer;
    bool mbIsAutoWalking;
    bool mbWeaponSuppressed;
    float mfLastHitTime;
    float mfLastWoundTime;
    float mfLastDamageTime;
    float mfWoundDamageAccumulator;
    float mfCamPullStrength;
    float mfCamPullDecay;
    bool mbCamHorizontalOnly;
    float mYawPull;
    float mfLastFacingImpulse;
    float mfPitch;
    float mfCreatureRideYaw;
    bool mbTurning;
    bool mbMoving;
    uint8_t meCreatureRideState[4];
    bool mbFoot;
    float mfBobAmount;
    int miGroundMat;
    bool mbJetpackIgnite;
    bool mbJetpackStart;
    float mfJetpackTimer;
    float mfJetpackTank;
    uint8_t mbFreeJetpack[4];
    float mfJetpackUpForce;
    float mfJetpackForce;
    float mfJetpackIgnitionForce;
    float mfJetpackUpBoost;
    float mfJetpackForwardBoost;
    float mfJetpackIgnitionBoost;
    float mfJetpackFreeDuration;
    float mfJetpackBoostTimer;
    bool mbJetpackBoostOverride;
    bool mbJetpackHeld;
    float mfRequiredJetpackRefillLevel;
    float mfFreeSprintTimer;
    float mfFreeSprintDuration;
    bool mbSprintIsFree;
    float mfAirTimer;
    float mfJetpackLandedTime;
    float mfActionTimer;
    bool mbActionRequiresButtonUp;
    bool mbJumpRequiresButtonUp;
    bool mbInteractBlockingAction;
    bool mbPreviouslyCouldNotFire;
    bool mbAltStarted;
    float mfRandomUnderwaterValue;
    eStaminaState meStaminaState;
    float mfStamina;
    float mfTurnAccelerator;
    bool mbIsTransitioning;
    float mfExertion;
    float mfExertionRate;
    float mfDampShoes;
    float mfHitReactFadeSpeed;
    PlayerMode mePlayerMode;
    bool mbIsDying;
    bool mbModRandomiseWeapon;
    float mfShieldChargeAccumulator;
    float mfShipShieldRechargeAccumulator;
    float mfLaunchThrustersAccumulator;
    float mfEnergyAccumulator;
    float mfEnergyPainTime;
    float mfLastScanTime;
    float mfLastShipScanTime;
    bool mbAimToggleActive;
    bool mbAimHeld;
    bool mbAimBeingHeld;
    bool mbHasFired;
    bool mbTorchActive;
    bool mbToggleTorch;
    bool mbTorchLightNeeded;
    bool mbForceLookAt;
    float mfLookAtTimeLeft;
    float mfLookAtTime;
    float mTimeLastUsedJetpack;
    float mfTimeInMeleeBoost;
    bool mbMeleeBoostActive;
    float mfTimeOnGround;
    float mfTimeRocketBootsActive;
    float mfRocketBootsBoostStrength;
    float mfRocketBootsHeightAdjust;
    float mfRocketBootsHeightAdjustVel;
    eRocketBoostState meRocketBootsState;
    float mfRocketBootsDoubleTapTimer;
    uint8_t meRocketBootsDoubleTapState[4];
    float mfTimeSicePredatorAttacked;
    FootstepOnDistanceTravel mLadderFootsteps;
    float mfDisabledTimer;
    float mfTimeUntilBodyRealignment;
    uint8_t muiCheckFallenThroughFloorCounter;
    int meSavedFilter;
    int meRequestedFilter;
    eCharacterMode meCharacterMode;
    eCharacterMode meRequestedCharacterMode;
    uint8_t mCurrentSlopeState[4];
    float mfTimeInCurrentSlopeState;
    sGrabbedObjectInfo maPreviousGrabbable[2];
    float mafGrabTimer[2];
    bool mabClenchingFist[2];
    sGrabbedObjectInfo maCurrentGrabbable[2];
    uint64_t mauPointingStartTime[2];
    uint64_t mauPointingEndTime[2];
    bool mbPendingFrameShift;
    bool mbHeadAnimationInProgress;
    eEmoteState meEmoteState;
    bool mbIsTargetingUnscannedPlanet;
    float mfShipRadiusCached;
    float mfFreighterMegaWarpTimer;
    uint64_t mu64LastAutoSaveTimeStamp;
    cGcSummonPetData mSummonPetData;
};
