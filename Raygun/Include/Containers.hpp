//
//  Containers.hpp
//  Raygun
//
//  Created by Owen Morgan on 28/03/2018.
//  Copyright © 2018 Owen Morgan. All rights reserved.
//

#pragma once

template <typename T>
struct array3 {
    T data[3];
    
    T& operator[] (size_t i) {
        return *(data + i);
    }
    
    const T& operator[] (size_t i) const {
        return *(data + i);
    }
};
