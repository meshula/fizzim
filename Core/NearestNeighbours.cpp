
#include "NearestNeighbours.h"

NearestNeighbours::	NearestNeighbours(PMath::Vec3f origin, PMath::Vec3f dimensions, int gridx, int gridy, int gridz)
: mp_DB(0)
{
	mp_DB = lqCreateDatabase(origin[0], origin[1], origin[2], dimensions[0], dimensions[1], dimensions[2], gridx, gridy, gridz);
}

NearestNeighbours::~NearestNeighbours()
{
	if (mp_DB) {
		lqDeleteDatabase(mp_DB);;
	}
}

void NearestNeighbours::AddProxy(NNProxy* pProxy)
{
	if (!pProxy->m_InDatabase) {
		lqInitClientProxy (&pProxy->m_Proxy, pProxy);
		pProxy->m_InDatabase = true;
		UpdateProxy(pProxy);
	}
}

void NearestNeighbours::RemoveProxy(NNProxy* pProxy)
{
	if (pProxy->m_InDatabase) {
		lqRemoveFromBin(&pProxy->m_Proxy);
		pProxy->m_InDatabase = false;
	}
}

void NearestNeighbours::UpdateProxy(NNProxy* pProxy)
{
	if (pProxy->m_InDatabase) {
		float const*const pPos = pProxy->GetPositionVectorPtr();
		lqUpdateForNewLocation(mp_DB, &pProxy->m_Proxy, pPos[0], pPos[1], pPos[2]);
	}
}



static uint32 s_SearchMask;
static NNProxy* sp_Ignore = 0;
static NNProxy* sp_Nearest = 0;
static float s_NearestDistanceSquared;

// called by LQ for each clientObject in the specified neighborhood:
// push that clientObject onto the ContentType vector in void*
// clientQueryState
static void perNeighborCallBackFunction  (void* clientObject,
                                            float distanceSquared,
                                            void* clientQueryState)		// client query state is the thing passed as last arg to lqMapOverAllObjectsInLocality
{
	NNProxy* pProxy = (NNProxy*) clientObject;
	if (pProxy != sp_Ignore) {
		if (pProxy->GetSearchMask() & s_SearchMask) {
			if (distanceSquared < s_NearestDistanceSquared) {
				s_NearestDistanceSquared = distanceSquared;
				sp_Nearest = pProxy;
			}
		}
	}
}


NNProxy*	NearestNeighbours::FindNearestNeighbour(
	Real		const*const pPosition,	// position to start search from
	Real		radius		,			// maximum search radius
	uint32		searchMask,
	NNProxy*	pExclude				// an ID to exclude
	)	
{
	s_NearestDistanceSquared = 1.0e8f;
	s_SearchMask = searchMask;
	sp_Ignore = pExclude;
	sp_Nearest = 0;
    lqMapOverAllObjectsInLocality (mp_DB, pPosition[0], pPosition[1], pPosition[2], radius,  
                                    perNeighborCallBackFunction,
                                    (void*) 0);

	return sp_Nearest;
}

