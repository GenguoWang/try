#ifndef _UTILITY_H_
#define _UTILITY_H_

template <typename _T>
void outputSet(std::set<_T> s)
{
    for(typename std::set<_T>::iterator it = s.begin();it != s.end();++it)
    {
        cout << *it << " ";
    }
    cout << endl;
}

#endif
