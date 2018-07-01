#ifndef ALG_BASE
#define	ALG_BASE

#include <map>
#include <vector>
#include <string>
class CAlgBase
{
public:
	CAlgBase();

	virtual ~CAlgBase();

	virtual bool createNewAlg() = 0;

	/*virtual bool LicenseCheck();*/
};

#endif // !ALG_BASE