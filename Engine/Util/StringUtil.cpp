//
//  StringUtil.cpp
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "StringUtil.h"

//NOTE: UNTESTED
void find_and_replace( std::string &source, const std::string find, std::string replace )
{
    size_t uPos = 0;
    const size_t uFindLen = find.length();
    const size_t uReplaceLen = replace.length();
    
    if( uFindLen == 0 )
    {
        return;
    }
    
    for( ;(uPos = source.find( find, uPos )) != std::string::npos; )
    {
        source.replace( uPos, uFindLen, replace );
        uPos += uReplaceLen;
    }
}
