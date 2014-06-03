#ifndef _PRIMEMANAGER_H_
#define _PRIMEMANAGER_H_
class PrimeManager
{
private:
    std::map<ZZ,ZZ> pm;
    PrimeRep pr;
    PrimeManager(PrimeManager &pm):pm(pm.pm),pr(pm.pr)
    {
        
    }
public:
    PrimeManager(PrimeRep &pr):pr(pr)
    {
        
    }
    std::set<ZZ> getPrime(const std::set<ZZ> &originSet)
    {
        std::set<ZZ> result;
        for(std::set<ZZ>::iterator it=originSet.begin();it != originSet.end();++it)
        {
            result.insert(getPrime(*it));
        }
        return result;
    }
    ZZ getPrime(const ZZ &origin)
    {
        if(pm.count(origin)>0)
        {
            return pm[origin];
        }
        else
        {
            ZZ p = pr.getPrime(origin);
            pm[origin] = p;
            return p;
        }
    }
};
#endif
