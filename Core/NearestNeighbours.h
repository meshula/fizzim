
/** @file	NearestNeighbours.h
	@brief	Nearest neighbour utilities
	*/

#ifndef _NEARESTNEIGHBOURS_H_
#define _NEARESTNEIGHBOURS_H_

#include "PMath.h"
#include "lq.h"

/// @class NNProxy
/// @brief virtual base class to help the proxy database

class NNProxy {
public:
	NNProxy() : m_InDatabase(false) { }
	virtual ~NNProxy() { }

	virtual float const*const	GetPositionVectorPtr() const = 0;
	virtual uint32	GetSearchMask() const = 0;

	friend class NearestNeighbours;

private:
	bool			m_InDatabase;
	lqClientProxy	m_Proxy;
};

/// @class	NearestNeighbours
/// @brief	a class which can find nearest neighbours on a 2D grid (z disregarded)

class NearestNeighbours {
public:
	NearestNeighbours(PMath::Vec3f origin, PMath::Vec3f dimensions, int gridx, int gridy, int gridz);
	~NearestNeighbours();

	void		AddProxy(NNProxy*);			///< add something to the database
	void		RemoveProxy(NNProxy*);		///< remove something from the database
	void		UpdateProxy(NNProxy*);		///< inform the database something has moved

	/// Find the nearest neighbours to the given location
	NNProxy*	FindNearestNeighbour(
		Real		const*const pPosition,	///< position to start search from
		Real		radius,					///< maximum search radius
		uint32		searchMask,				///< a mask of entities to consider in the search
		NNProxy*	pExclude				///< an ID to exclude
		);

private:
	lqDB*	mp_DB;
};

#endif
