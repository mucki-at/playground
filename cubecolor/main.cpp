//
//  main.cpp
//  cubecolor
//
//  Created by Michael Lacher on 5/8/24.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <queue>

int usage(const char* name)
{
    std::cout << "usage: " << name << " <number of dimensions>" << std::endl;
    return 1;
}

inline int p2(int i) { return 1 << i; }

int dimensions = 0;



void printBin(int v)
{
    for (int bit = 1; bit < p2(dimensions); bit<<=1)
    {
        if (v & bit)  std::cout << '1';
        else std::cout << '0';
    }

}

uint minHamming(const std::vector<int>& va)
{
    uint min=uint(-1);
    
    for (int a : va)
        for (int b: va)
        {
            if (b==a) continue;
            
            uint h = __builtin_popcount(a ^ b);
            if (h<min) min=h;
        }
    
    return min;
}

uint minHamming(const std::vector<int>& va, const std::vector<int>& vb)
{
    uint min=uint(-1);
    
    for (int a : va)
        for (int b: vb)
        {
            uint h = __builtin_popcount(a ^ b);
            if (h<min) min=h;
        }
    
    return min;
}

struct Color
{
    int self = -1;
    int options = 0;
    int needed = 0;
};

int main(int argc, const char * argv[])
{
    if (argc != 2) return usage(argv[0]);
    dimensions = atoi(argv[1]);
    if (dimensions <= 0) return usage(argv[0]);
    
    std::vector<Color> colors;
    colors.resize(p2(dimensions));
    for (auto& c : colors) c.options = c.needed = p2(dimensions)-1;
    
    std::queue<int> todo;
    todo.push(0);

    while (!todo.empty())
    {
        while (!todo.empty())
        {
            int vertex = todo.front();
            todo.pop();
            
            Color& c = colors[vertex];
            
            uint options = c.options;
            uint freeNeighbors = 0;
            // check the requirements of all neighbors
            for (int bit = 1; bit < p2(dimensions); bit<<=1)
            {
                Color& n = colors[vertex ^ bit];
                options &= n.needed;
                if (n.self < 0) freeNeighbors |= bit;   // remember which neighbors need updating
            }
            
            int newColor = __builtin_ffs(options);  // pick the first available option
            if (newColor == 0)
            {
                std::cerr << "No color options left for vertex " << vertex << std::endl;
                continue;
            }
            else
            {
                c.self = newColor-1;
                c.options = p2(c.self);
                
                c.needed &= ~c.options; // we do not need the color we are outselfs
                
                for (int bit = 1; bit < p2(dimensions); bit<<=1)
                {
                    colors[vertex ^ bit].needed &= ~c.options;  // we no longer need this color
                    if (freeNeighbors & bit) todo.push(vertex ^ bit);  // this neighbor still needs work
                }
            }
        }
        
        for (int i=0; i<colors.size(); ++i)
        {
            if (colors[i].needed != 0)
            {
                for (int bit = 1; bit < p2(dimensions); bit<<=1)
                {
                    Color& n = colors[i ^ bit];
                    if (n.self==-1)
                    {
                        n.self = __builtin_ffs(colors[i].needed)-1;
                        n.options = p2(n.self);
                        n.needed &= ~n.options;
                        for (int bit = 1; bit < p2(dimensions); bit<<=1)
                        {
                            colors[i ^ bit].needed &= ~n.options;  // we no longer need this color
                            if (colors[i ^ bit].self<0) todo.push(i ^ bit);  // this neighbor still needs work
                        }
                        
                        break;
                   }
                }
                if (todo.empty())
                {
                    std::cerr << "Vertex " << i << " needs color "; printBin(colors[i].needed); std::cout << " but no candidate neighbour found." << std::endl;
                }
           }
        }
    }

    
    std::vector<std::vector<int>> results;
    results.resize(dimensions+1);
    for(int i=0; i<colors.size(); ++i)
    {
        int c = colors[i].self;
        if (c<0 || c>dimensions) c=dimensions;
        results[c].push_back(i);
    }
    for(int color=0; color<dimensions; ++color)
    {
        std::cout << char('A'+color) << ": ";
        for (int i:results[color]) { printBin(i); std::cout << ", "; }
        std::cout << std::endl;
    }
    if (results[dimensions].empty())
    {
        std::vector<uint> hammings;
        hammings.resize(dimensions * dimensions, 0);
        for (int i=0; i<dimensions; ++i)
            for (int j=i; j<dimensions; ++j)
            {
                int dist = i==j ? minHamming(results[i]) : minHamming(results[i], results[j]);
                hammings[i*dimensions + j] = dist;
                hammings[j*dimensions + i] = dist;
            }
        
        std::cout << "minimum hamming distances" << std::endl;
        for (int i=0; i<dimensions; ++i)
        {
            for (int j=0; j<dimensions; ++j)
            {
                std::cout << hammings[i*dimensions + j] << ", ";
            }
            std::cout << std::endl;
        }

    }
    else
    {
        std::cout << "Error: ";
        for(int i:results[dimensions]) { printBin(i); std::cout << ", "; }
        std::cout << std::endl;
    }
    
    return 0;
}
