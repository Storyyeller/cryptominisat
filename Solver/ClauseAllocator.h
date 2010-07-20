/***********************************************************************
CryptoMiniSat -- Copyright (c) 2009 Mate Soos

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/

#ifndef CLAUSEALLOCATOR_H
#define CLAUSEALLOCATOR_H

#ifdef _MSC_VER
#include <msvc/stdint.h>
#else
#include <stdint.h>
#endif //_MSC_VER

#include "Vec.h"
#include <map>
#include <vector>
using std::map;
using std::vector;

#ifdef USE_BOOST
#include <boost/pool/pool.hpp>
#endif //USE_BOOST

#define NUM_BITS_OUTER_OFFSET 4

class Clause;
class XorClause;
class Solver;

typedef uint32_t ClauseOffset;

class ClauseAllocator {
    public:
        ClauseAllocator();
        ~ClauseAllocator();
        
        template<class T>
        Clause* Clause_new(const T& ps, const uint group, const bool learnt = false);
        template<class T>
        XorClause* XorClause_new(const T& ps, const bool inverted, const uint group);
        Clause* Clause_new(Clause& c);

        const ClauseOffset getOffset(const Clause* ptr) const;

        inline Clause* getPointer(const uint32_t offset)
        {
            return (Clause*)(dataStarts[offset&((1 << NUM_BITS_OUTER_OFFSET) - 1)]
                            +(offset >> NUM_BITS_OUTER_OFFSET));
        }

        void clauseFree(Clause* c);

        void consolidate(Solver* solver);

    private:
        uint32_t getOuterOffset(const Clause* c) const;
        uint32_t getInterOffset(const Clause* c, const uint32_t outerOffset) const;
        const ClauseOffset combineOuterInterOffsets(const uint32_t outerOffset, const uint32_t interOffset) const;

        template<class T>
        void updatePointers(vec<T*>& toUpdate, const map<Clause*, Clause*>& oldToNewPointer);
        void updatePointers(vector<Clause*>& toUpdate, const map<Clause*, Clause*>& oldToNewPointer);
        void updatePointers(vector<XorClause*>& toUpdate, const map<Clause*, Clause*>& oldToNewPointer);
        void updatePointers(vector<std::pair<Clause*, uint> >& toUpdate, const map<Clause*, Clause*>& oldToNewPointer);

        template<class T>
        void updateOffsets(vec<vec<T> >& watches, const map<ClauseOffset, ClauseOffset>& oldToNewOffset);
        
        vec<uint32_t*> dataStarts;
        vec<size_t> sizes;
        vec<vec<uint32_t> > origClauseSizes;
        vec<size_t> maxSizes;
        vec<size_t> currentlyUsedSize;
        vec<uint32_t> origSizes;

        #ifdef USE_BOOST
        boost::pool<> clausePoolBin;
        #endif //USE_BOOST

        void* allocEnough(const uint32_t size);
};

#endif //CLAUSEALLOCATOR_H
