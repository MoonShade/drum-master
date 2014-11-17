#include "StdAfx.h"







CDiff::CDiff()
	: m_Loaded(false), m_Data(NULL), m_ID(0), m_BID(0)
{
	for(u32 i = 0; i < 9; i++)
	{
		SpeedDifficulty[i] = -1;
		JumpDifficulty[i] = -1;
	}
}


CDiff::~CDiff()
{
	unload();
}


void CDiff::loadDummy()
{
	unload();

	m_Data = new DiffData;


	m_Loaded = true;
}

void CDiff::load()
{
	unload();

	read(m_Path);
	init();
	//calcLevel(g_pGame->get_ModValue());
	preprocess(g_pGame->get_ModValue());


	m_Loaded = true;
}


void CDiff::unload()
{
	delete m_Data;
	m_Data = NULL;

	m_Loaded = false;
}



static f64 FactorialLookup[33];


// just check if n is appropriate, then return the result
f64 factorial(int n)
{
	if (n < 0) { __LOG(MSG_CRITERROR, "Factorial < 0. (%d)", n); return 1; }
	if (n > 32)
	{
		__LOG(MSG_CRITERROR, "Factorial > 32. (%d)", n);
		return 1;
	}

	return FactorialLookup[n]; /* returns the value n! as a SUMORealing point number */
}




// create lookup table for fast factorial calculation
void CreateFactorialTable()
{
	// fill untill n=32. The rest is too high to represent
	FactorialLookup[0] = 1.0;
	FactorialLookup[1] = 1.0;
	FactorialLookup[2] = 2.0;
	FactorialLookup[3] = 6.0;
	FactorialLookup[4] = 24.0;
	FactorialLookup[5] = 120.0;
	FactorialLookup[6] = 720.0;
	FactorialLookup[7] = 5040.0;
	FactorialLookup[8] = 40320.0;
	FactorialLookup[9] = 362880.0;
	FactorialLookup[10] = 3628800.0;
	FactorialLookup[11] = 39916800.0;
	FactorialLookup[12] = 479001600.0;
	FactorialLookup[13] = 6227020800.0;
	FactorialLookup[14] = 87178291200.0;
	FactorialLookup[15] = 1307674368000.0;
	FactorialLookup[16] = 20922789888000.0;
	FactorialLookup[17] = 355687428096000.0;
	FactorialLookup[18] = 6402373705728000.0;
	FactorialLookup[19] = 121645100408832000.0;
	FactorialLookup[20] = 2432902008176640000.0;
	FactorialLookup[21] = 51090942171709440000.0;
	FactorialLookup[22] = 1124000727777607680000.0;
	FactorialLookup[23] = 25852016738884976640000.0;
	FactorialLookup[24] = 620448401733239439360000.0;
	FactorialLookup[25] = 15511210043330985984000000.0;
	FactorialLookup[26] = 403291461126605635584000000.0;
	FactorialLookup[27] = 10888869450418352160768000000.0;
	FactorialLookup[28] = 304888344611713860501504000000.0;
	FactorialLookup[29] = 8841761993739701954543616000000.0;
	FactorialLookup[30] = 265252859812191058636308480000000.0;
	FactorialLookup[31] = 8222838654177922817725562880000000.0;
	FactorialLookup[32] = 263130836933693530167218012160000000.0;
}




f64 Ni(int n, int i)
{
	f64 ni;
	f64 a1 = factorial(n);
	f64 a2 = factorial(i);
	f64 a3 = factorial(n - i);
	ni =  a1/ (a2 * a3);
	return ni;
}

// Calculate Bernstein basis
f64 Bernstein(int n, int i, double t)
{
	f64 basis;
	f64 ti; /* t^i */
	f64 tni; /* (1 - t)^i */

	/* Prevent problems with pow */

	if(t == 0.0 && i == 0) 
	{
		ti = 1.0; 
	}
	else 
	{
		ti = pow(t, i);
	}

	if(n == i && t == 1.0) 
	{
		tni = 1.0;
	}
	else 
	{
		tni = pow((1 - t), (n - i));
	}

	//Bernstein basis
	basis = Ni(n, i) * ti * tni; 
	return basis;
}


void Bezier2D(vector<vector2d<f64>>& cpoints_vector, int larppoints, vector<vector2d<f64>>& larp_vector)
{

	u32 larppoints_start = larp_vector.size();

	int npts = cpoints_vector.size();
	int icount, jcount;
	double step, t;

	// Calculate points on curve

	icount = 0;
	t = 0;
	step = (double)1.0 / (larppoints - 1);



	for (int i1 = 0; i1 != larppoints; i1++)
	{ 
		if ((1.0 - t) < 0.01) 
			t = 1.0;

		jcount = 0;
		larp_vector.push_back(vector2d<f64>(0.0, 0.0));

		for (int i = 0; i != npts; i++)
		{
			double basis = Bernstein(npts - 1, i, t);
			larp_vector[larppoints_start+icount].X += basis * cpoints_vector[jcount].X;
			larp_vector[larppoints_start+icount].Y += basis * cpoints_vector[jcount].Y;
			jcount++;
		}

		icount++;
		t += step;
	}
}







bool IsFlatEnough(vector<vector2d<f64>>& curve)
{

	static f64 length;
	static f64 pos;
	static f64 max;
	static f64 res;	
	static u32 i;

	// Calc length
	length = 0;
	for(i = 1; i < curve.size(); i++)
	{
		length += curve[i-1].getDistanceFrom(curve[i]);
	}


#ifndef __APPROX_LENGTH

	pos = 0;
	max = 0;
	res = 0;


	for(i = 1; i < curve.size()-1; i++)
	{
		pos += curve[i].getDistanceFrom(curve[i-1]);

		res = curve[i].getDistanceFrom(curve[0] + (curve.back() - curve.front()) * (pos / length));


		if(res > max)
			max = res;
	}
	

	if(max > __APPROX_TOLERANCE)
	{
		return false;
	}

#else



	if(length - curve.front().getDistanceFrom(curve.back()) > __APPROX_TOLERANCE)
	{
		return false;
	}

#endif

	return true;
}



void Subdivide(vector<vector2d<f64>>& cpoints_vector, vector<vector2d<f64>>& l, vector<vector2d<f64>>& r)
{


	// Calc middle points... GEEEEZ
	vector<vector2d<f64>>* midpoints;
	u32 len = cpoints_vector.size()-1;
	midpoints = new vector<vector2d<f64>>[len];


	for(u32 i = 1; i < cpoints_vector.size(); i++)
	{
		midpoints[0].push_back((cpoints_vector[i] + cpoints_vector[i-1]) / 2);
	}

	for(u32 i = 1; i < len; i++)
	{
		for(u32 j = 1; j < midpoints[i-1].size(); j++)
		{
			midpoints[i].push_back((midpoints[i-1][j-1] + midpoints[i-1][j]) / 2);
		}
	}


	// Fill L & R
	l.push_back(cpoints_vector[0]);
	for(u32 i = 0; i < len; i++)
	{
		l.push_back(midpoints[i].front());
	}

	
	for(s32 i = (s32)len-1; i > -1; i--)
	{
		r.push_back(midpoints[i].back());
	}
	r.push_back(cpoints_vector[len]);


	delete [] midpoints;
}

void Flatten(vector<vector2d<f64>>& cpoints_vector, vector<vector2d<f64>>& larp_vector)
{

	if(IsFlatEnough(cpoints_vector))
	{
		larp_vector.push_back(cpoints_vector.back());
		//printf(" %f %f\n", cpoints_vector[cpoints_vector.size()-1].X, cpoints_vector[cpoints_vector.size()-1].Y);
	}
	else
	{
		vector<vector2d<f64>> l;
		vector<vector2d<f64>> r;

		Subdivide(cpoints_vector, l, r);

		Flatten(l, larp_vector);
		Flatten(r, larp_vector);
	}

}


void HitObject::bezier()
{
	u32 larppoints_start = 0;
	for(u32 i = 0; i < slider->points.size(); i++)
	{
		// Find amount of points
		u32 amount = 0;
		for(; i+amount+1 < slider->points.size() && slider->points[i+amount] != slider->points[i+amount+1]; amount++)
		{

		}


		vector<vector2d<f64>> cpoints_vector;
		for(u32 j = 0; j < amount+1; j++)
		{
			cpoints_vector.push_back(slider->points[i+j]);
		}


		// Use polynomial representation with precomputed factorials
		if(amount <= 32)
		{
			u32 larppoints = 2;
			if(amount > 1)
				larppoints = 10 + 4*amount;


			Bezier2D(cpoints_vector, larppoints, larp);
		}
		// Approximate using a recursive flatten method
		else
		{
			larp.push_back(slider->points[i]);
			Flatten(cpoints_vector, larp);
		}

		


		
		i += amount;
	}
}




bool lineIntersection(vector2d<f64> A, vector2d<f64> B, vector2d<f64> C, vector2d<f64> D, vector2d<f64>& center)
{
	double  distAB, theCos, theSin, newX, ABpos ;

	//  Fail if either line is undefined.
	if (A == B || C == D)
		return false;

	//  (1) Translate the system so that point A is on the origin.
	B -= A;
	C -= A;
	D -= A;

	//  Discover the length of segment A-B.

	distAB = B.getLength();

	//  (2) Rotate the system so that point B is on the positive X axis.
	theCos = B.X/distAB;
	theSin = B.Y/distAB;
	newX=C.X*theCos+C.Y*theSin;
	C.Y  =C.Y*theCos-C.X*theSin; C.X=newX;
	newX=D.X*theCos+D.Y*theSin;
	D.Y  =D.Y*theCos-D.X*theSin; D.X=newX;

	//  Fail if the lines are parallel.
	if (C.Y==D.Y) return false;

	//  (3) Discover the position of the intersection point along line A-B.
	ABpos=D.X+(C.X-D.X)*D.Y/(D.Y-C.Y);

	//  (4) Apply the discovered position to line A-B in the original coordinate system.
	center.X=A.X+ABpos*theCos;
	center.Y=A.Y+ABpos*theSin;

	//  Success.
	return true;
}


char* name;

void HitObject::circle()
{
	vector2d<f64>& A = slider->points[0];
	vector2d<f64>& B = slider->points[1];
	vector2d<f64>& C = slider->points[2];

	vector2d<f64> center;


	vector2d<f64> MidAB = (A + B) / 2;
	vector2d<f64> MidBC = (B + C) / 2;

	vector2d<f64> DirAB = (A - B).rotateBy(90.0f);
	vector2d<f64> DirBC = (B - C).rotateBy(90.0f);



	if(!lineIntersection(MidAB, MidAB+DirAB, MidBC, MidBC+DirBC, center))
	{
		// Apply bezier instead!
		__LOG_DBG(MSG_WARNING, "LI FAILED: %02d:%02d:%04d : %s", (timeOrig / 1000) / 60, (timeOrig / 1000) % 60, timeOrig % 1000, name);

		bezier();
	}
	else
	{
		f64 angle = 2*(DirAB.getAngle() - DirBC.getAngle());
		if(angle > 360)
		{
			while(angle > 0)
			{
				angle -= 360;
			}
		}

		if(angle < -360)
		{
			while(angle < 0)
			{
				angle += 360;
			}
		}
		
		f64 step = angle / 20;

		if(abs(step * (center-A).getLengthSQ()) > 1000000)
		{
			__LOG_DBG(MSG_WARNING, "TOO BIG CIRCLE: %02d:%02d:%04d : %s", (timeOrig / 1000) / 60, (timeOrig / 1000) % 60, timeOrig % 1000, name);
			bezier();
		}
		else
		{
			vector2d<f64> point = A;
			for(u32 i = 0; i < 20; i++)
			{
				larp.push_back(point);
				point.rotateBy(step, center);
			}


			larp.push_back(point);
		}


	}
	

	
	

}


bool compareHO(HitObject* d1, HitObject* d2)
{
	return (d1->timeOrig < d2->timeOrig);
}





static char szBuf[0xFFFFFF];

s32 CDiff::read(const char* filename, const char* full_filename)
{
	strcpy(m_Name, filename);
	m_Name[strlen(m_Name)-4] = '\0';

	return read(full_filename);
}

s32 CDiff::read(const char* full_filename)
{
	strcpy(m_Path, full_filename);

	m_Crc = g_CRC.CalcHash(m_Path);


	FILE* pFile;
	fopen_s(&pFile, full_filename, "r");



	if(pFile == NULL)
	{
		__LOG(MSG_ERROR, "File could not be opened. \"%s\"", full_filename);
		return -1;
	}


	m_Data = new DiffData;






	// Read the file
	DWORD dwRes = fread(szBuf, sizeof(char), 0xFFFFFF, pFile);
	szBuf[dwRes] = '\0';


	DWORD dwPos = 0;

	// Find osuFileFormatVersion
	for(; dwPos < dwRes; dwPos++)
	{
		if(szBuf[dwPos] >= '0' && szBuf[dwPos] <= '9')
			break;
	}

	// Read Version
	if(sscanf(&szBuf[dwPos], "%d", &m_Data->osuFileFormatVersion) != 1)
	{
		__LOG(MSG_ERROR, "No file format version. \"%s\"", m_Name);
		fclose(pFile);
		return -1;
	}


	

	// In case of old maps it wont find a mode / leniency

	m_Mode = 0;
	m_Data->StackLeniency = 0;

	// Find beginning of metadata settings
	bool test = false;
	for(; dwPos < dwRes; dwPos++)
	{
		if(!strncmp("StackLeniency:", &szBuf[dwPos], 14))
		{
			dwPos += 14;
			// Find number
			for(; dwPos < dwRes; dwPos++)
			{
				if(szBuf[dwPos] >= '0' && szBuf[dwPos] <= '9')
					break;
			}

			if(sscanf(&szBuf[dwPos], "%f", &m_Data->StackLeniency) != 1)
			{
				__LOG(MSG_ERROR, "No StackLeniency found. \"%s\"", m_Name);
				fclose(pFile);
				return -1;
			}
		}

		if(!strncmp("Mode:", &szBuf[dwPos], 5))
		{
			dwPos += 5;
			// Find number
			for(; dwPos < dwRes; dwPos++)
			{
				if(szBuf[dwPos] >= '0' && szBuf[dwPos] <= '9')
					break;
			}

			if(sscanf(&szBuf[dwPos], "%d", &m_Mode) != 1)
			{
				__LOG(MSG_ERROR, "No gamemode found. \"%s\"", m_Name);
				fclose(pFile);
				return -1;
			}

			/*if(m_Data->Mode != 0)
			{
				fclose(pFile);
				return 0;
			}*/
		}

		if(!strncmp("[Metadata]", &szBuf[dwPos], 10))
		{
			test = true;
			dwPos += 11;
			break;
		}
	}



	


	for(; dwPos < dwRes; dwPos++)
	{
		char buf[256];

		// Read title
		
		if(!strncmp("Title:", &szBuf[dwPos], 6))
		{

			dwPos += 6;
			u32 i;
			for(i = 0; dwPos < dwRes && szBuf[dwPos] != '\n'; dwPos++)
			{
				buf[i] = szBuf[dwPos];
				i++;
			}
			buf[i] = '\0';

			m_Data->Name = new char[i+1];
			strcpy(m_Data->Name, buf);

		}

		// Read artist

		if(!strncmp("Artist:", &szBuf[dwPos], 7))
		{


			dwPos += 7;
			u32 i;
			for(i = 0; dwPos < dwRes && szBuf[dwPos] != '\n'; dwPos++)
			{
				buf[i] = szBuf[dwPos];
				i++;
			}
			buf[i] = '\0';

			m_Data->Artist = new char[i+1];
			strcpy(m_Data->Artist, buf);

		}


		// Read creator

		if(!strncmp("Creator:", &szBuf[dwPos], 8))
		{


			dwPos += 8;
			u32 i;
			for(i = 0; dwPos < dwRes && szBuf[dwPos] != '\n'; dwPos++)
			{
				buf[i] = szBuf[dwPos];
				i++;
			}
			buf[i] = '\0';

			m_Data->Creator = new char[i+1];
			strcpy(m_Data->Creator, buf);

		}


		// Read diffname

		if(!strncmp("Version:", &szBuf[dwPos], 8))
		{


			dwPos += 8;
			u32 i;
			for(i = 0; dwPos < dwRes && szBuf[dwPos] != '\n'; dwPos++)
			{
				buf[i] = szBuf[dwPos];
				i++;
			}
			buf[i] = '\0';

			m_Data->Diffname = new char[i+1];
			strcpy(m_Data->Diffname, buf);

		}


	
		// Read source

		if(!strncmp("Source:", &szBuf[dwPos], 7))
		{


			dwPos += 7;
			u32 i;
			for(i = 0; dwPos < dwRes && szBuf[dwPos] != '\n'; dwPos++)
			{
				buf[i] = szBuf[dwPos];
				i++;
			}
			buf[i] = '\0';

			m_Data->Source = new char[i+1];
			strcpy(m_Data->Source, buf);
		}
		



		// Read tag

		if(!strncmp("Tags:", &szBuf[dwPos], 5))
		{


			dwPos += 5;
			u32 i;
			for(i = 0; dwPos < dwRes && szBuf[dwPos] != '\n'; dwPos++)
			{
				if(szBuf[dwPos] == ' ')
				{
					buf[i] = '\0';
					m_Data->Tags.push_back(new char[i+1]);
					strcpy(m_Data->Tags.back(), buf);
					i = 0;
				}
				else
				{
					buf[i] = szBuf[dwPos];
					i++;
				}
			}

		}
		

		if(!strncmp("[Difficulty]", &szBuf[dwPos], 12))
		{
			dwPos += 13;
			break;
		}
		
	}

	
	if(m_Data->Name == NULL || m_Data->Artist == NULL || m_Data->Creator == NULL || m_Data->Diffname == NULL)
	{
		__LOG(MSG_ERROR, "No metadata. %d \"%s\"", test, m_Name);
		fclose(pFile);
		return -1;
	}





	// Read Drain
	if(sscanf(&szBuf[dwPos], "HPDrainRate:%d", &m_Data->HPDrainRateOrig) != 1)
	{
		__LOG(MSG_ERROR, "No HPDrainRate. \"%s\"", m_Name);
		fclose(pFile);
		return -1;
	}

	// Go look for end of the line
	for(; dwPos < dwRes; dwPos++)
	{
		if(szBuf[dwPos] == '\n')
		{
			dwPos++;
			break;
		}
	}


	// Read CS
	if(sscanf(&szBuf[dwPos], "CircleSize:%d", &m_Data->CircleSizeOrig) != 1)
	{
		__LOG(MSG_ERROR, "No CircleSize. \"%s\"", m_Name);
		fclose(pFile);
		return -1;
	}

	// Go look for end of the line
	for(; dwPos < dwRes; dwPos++)
	{
		if(szBuf[dwPos] == '\n')
		{
			dwPos++;
			break;
		}
	}


	// Read OD
	if(sscanf(&szBuf[dwPos], "OverallDifficulty:%d", &m_Data->OverallDifficultyOrig) != 1)
	{
		__LOG(MSG_ERROR, "No OverallDifficulty. \"%s\"", m_Name);
		fclose(pFile);
		return -1;
	}

	// Go look for end of the line
	for(; dwPos < dwRes; dwPos++)
	{
		if(szBuf[dwPos] == '\n')
		{
			dwPos++;
			break;
		}
	}

	// Read AR
	if(sscanf(&szBuf[dwPos], "ApproachRate:%d", &m_Data->ApproachRateOrig) != 1)
	{
		m_Data->ApproachRateOrig = m_Data->OverallDifficultyOrig;
	}
	else
	{
		// Go look for end of the line
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == '\n')
			{
				dwPos++;
				break;
			}
		}
	}


	// Read SM
	if(sscanf(&szBuf[dwPos], "SliderMultiplier:%f", &m_Data->SliderMultiplier) != 1)
	{
		__LOG(MSG_ERROR, "No SliderMultiplier. \"%s\"", m_Name);
		fclose(pFile);
		return -1;
	}

	// Go look for end of the line
	for(; dwPos < dwRes; dwPos++)
	{
		if(szBuf[dwPos] == '\n')
		{
			dwPos++;
			break;
		}
	}


	// Read SliderTickRate
	if(sscanf(&szBuf[dwPos], "SliderTickRate:%f", &m_Data->SliderTickRate) != 1)
	{
		__LOG(MSG_ERROR, "No SliderTickRate. \"%s\"", m_Name);
		fclose(pFile);
		return -1;
	}

	m_Data->SliderTickRate = clamp((float)m_Data->SliderTickRate, 0.5f, 8.0f);

	// Go look for end of the line
	for(; dwPos < dwRes; dwPos++)
	{
		if(szBuf[dwPos] == '\n')
		{
			dwPos++;
			break;
		}
	}




	// TimingPoints
	bool found = false;
	// Find beginning of timing points list
	for(; dwPos < dwRes-15; dwPos++)
	{
		if(!strncmp("[TimingPoints]", &szBuf[dwPos], 14))
		{
			// Skip the 'TimingPoints' AND the linebreak
			dwPos += 15;
			found = true;
			break;
		}
	}

	if(found == false)
	{
		__LOG(MSG_ERROR, "No TimingPoints. \"%s\"", m_Name);
		fclose(pFile);
		return -1;
	}


	// Each line
	while(true)
	{


		TimingPoint* TP = new TimingPoint();


		TP->time = atoi(&szBuf[dwPos]);

		// comma...
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == ',')
			{
				dwPos++;
				break;
			}
		}

		TP->Number = (f64)atof(&szBuf[dwPos]);
		
		if(TP->Number == 0)
		{
			__LOG(MSG_ERROR, "NULL-number in \"%s\".", m_Name);
		}

		// comma...
		for(; dwPos < dwRes; dwPos++)
		{

			if(szBuf[dwPos] == ',')
			{
				dwPos++;
				break;
			}
			else if(szBuf[dwPos] == '\n')
			{
				goto l_TP_linebreak;
			}
		}


		// SnapDivisor

		// comma...
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == ',')
			{
				dwPos++;
				break;
			}
			else if(szBuf[dwPos] == '\n')
			{
				goto l_TP_linebreak;
			}
		}

		
		// SampleSet

		// comma...
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == ',')
			{
				dwPos++;
				break;
			}
			else if(szBuf[dwPos] == '\n')
			{
				goto l_TP_linebreak;
			}
		}


		// CustomSet



		// comma...
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == ',')
			{
				dwPos++;
				break;
			}
			else if(szBuf[dwPos] == '\n')
			{
				goto l_TP_linebreak;
			}
		}

		// Volume
		



		// comma...
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == ',')
			{
				dwPos++;
				break;
			}
			else if(szBuf[dwPos] == '\n')
			{
				goto l_TP_linebreak;
			}
		}


		TP->inherited = !atoi(&szBuf[dwPos]);

		// comma...
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == ',')
			{
				dwPos++;
				break;
			}
			else if(szBuf[dwPos] == '\n')
			{
				goto l_TP_linebreak;
			}
		}

		// IsKiai (fuck kiai)


l_TP_linebreak:


		m_Data->TimingPoints.push_back(TP);





		// Go look for end of the line
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == '\n')
			{
				dwPos++;
				break;
			}
		}


		for(; dwPos < dwRes && irr::core::isspace(szBuf[dwPos]); dwPos++)
		{
		}


		if(szBuf[dwPos] < '0' || szBuf[dwPos] > '9')
		{
			goto l_HitObjects;
		}


		if(dwPos >= dwRes)
		{
			__LOG(MSG_ERROR, "Empty after timingpoints. (%d) \"%s\"", m_Data->TimingPoints.size(), m_Name);



			fclose(pFile);
			return -1;
		}
	}

l_HitObjects:

	
	
	found = false;
	// Find beginning of beat list
	for(; dwPos < dwRes-13; dwPos++)
	{
		if(!strncmp("[HitObjects]", &szBuf[dwPos], 12))
		{
			// Skip the 'HitObjects' AND the linebreak
			dwPos += 13;
			found = true;
			break;
		}
	}

	if(found == false)
	{
		__LOG(MSG_ERROR, "No HitObjects. \"%s\"", m_Name);
		fclose(pFile);
		return -1;
	}

	// Each line
	s32 buf;
	s32 typeNum;
	while(true)
	{
		HitObject* HO = new HitObject();

		if(sscanf(&szBuf[dwPos], "%f,%f,%d,%d,%d,%d", &HO->posOrig.X, &HO->posOrig.Y, &HO->timeOrig, &typeNum, &buf, &HO->durationOrig) < 4)
		{
			// Obv this is not right... assume we are finished with this beatmap and break.
			//__LOG(MSG_ERROR, "Weird hitobject 1. \"%s\"", m_Name);
			delete HO;
			break;
		}



		HO->type = HO_CIRCLE;

		switch(typeNum)
		{


		case 8:
		case 12:
		case 28:
		case 44:
		case 60:
		case 76:
		case 92:
		case 108:
		case 124:

			HO->type = HO_SPINNER;
			break;


		default:

			HO->type = HO_CIRCLE;
			break;

		}


		if(HO->type == HO_SPINNER)
		{
			HO->durationOrig -= HO->timeOrig;
		}

		// Go look for end of the line
		for(; dwPos < dwRes; dwPos++)
		{
			if(szBuf[dwPos] == '\n' || szBuf[dwPos] == '|')
			{
				break;
			}
		}



		if(dwPos >= dwRes)
		{
			m_Data->HitObjects.push_back(HO);
			break;
		}

		if(szBuf[dwPos] == '|')
		{
			HO->type = HO_SLIDER;

			HO->slider = new Slider;
			HO->slider->type = szBuf[dwPos-1];

			if(HO->slider->type != 'P' && HO->slider->type != 'B' &&
				HO->slider->type != 'L' && HO->slider->type != 'C')
			{
				__LOG(MSG_ERROR, "Unknown slider type '%c'. \"%s\"", HO->slider->type, m_Name);
				delete HO;
				fclose(pFile);
				return -1;
			}


			HO->slider->points.push_back(vector2d<f64>((f32)HO->posOrig.X, (f32)HO->posOrig.Y));

			f64 X, Y;
			while(szBuf[dwPos] == '|')
			{
				X = (f64)atoi(&szBuf[++dwPos]);
				for(; dwPos < dwRes; dwPos++)
				{
					if(szBuf[dwPos] == ':')
					{
						break;
					}
				}

				if(dwPos == dwRes)
				{
					__LOG(MSG_ERROR, "Weird hitobject 2. \"%s\"", m_Name);
					delete HO;
					fclose(pFile);
					return -1;
				}

				Y = (f64)atoi(&szBuf[++dwPos]);

				for(; dwPos < dwRes; dwPos++)
				{
					if(szBuf[dwPos] == '|' || szBuf[dwPos] == ',')
					{
						break;
					}
				}

				if(dwPos == dwRes)
				{
					__LOG(MSG_ERROR, "Weird hitobject 3. \"%s\"", m_Name);
					delete HO;
					fclose(pFile);
					return -1;
				}

				HO->slider->points.push_back(vector2d<f64>(X, Y));

				if(HO->slider->type == 'B')
				{
					u32 amount = 0;
					for(u32 i = 0; i < HO->slider->points.size(); i++)
					{
						amount++;

						/*if(amount > 32)
						{
							__LOG(MSG_ERROR, "Too many sliderpoints on bezier (>32). \"%s\"", filename);
							fclose(pFile);
							return -1;
						}*/

						if(HO->slider->points.size() > i+1 && HO->slider->points[i] == HO->slider->points[i+1])
							amount = 0;
					}
				}
			}

			dwPos++;
			
			HO->slider->repetitions = atoi(&szBuf[dwPos]);

			for(; dwPos < dwRes; dwPos++)
			{
				if(szBuf[dwPos] == ',')
				{
					break;
				}
			}

			if(dwPos == dwRes)
			{
				__LOG(MSG_ERROR, "Weird hitobject 4. \"%s\"", m_Name);
				delete HO;
				fclose(pFile);
				return -1;
			}

			HO->slider->length = (f32)atof(&szBuf[++dwPos]);
		}


		m_Data->HitObjects.push_back(HO);


		// Go look for end of the line
		for(; dwPos < dwRes; dwPos++)
		{

			if(szBuf[dwPos] == '\n')
			{
				dwPos++;
				break;
			}
		}


		if(dwPos >= dwRes)
			break;
	}
	


	fclose(pFile);


	char num[64];

	m_BID = 0;

	// Get BID if existing
	s32 i = strlen(m_Path)-1;
	while(i >= 0 && m_Path[i] != '\\' && m_Path[i] != '/')
	{
		i--;
	}

	i++;

	if(m_Path[i] >= '0' && m_Path[i] <= '9')
	{
		s32 j = 0;
		while(m_Path[i + j] != ' ')
		{
			num[j] = m_Path[i+j];
			j++;
		}

		num[j] = '\0';

		m_BID = atoi(num);
	}



	m_ID = 0;

	// Get ID if existing
	i = strlen(m_Path)-1;
	while(i >= 0 && m_Path[i] != '\\' && m_Path[i] != '/')
	{
		i--;
	}

	if(i > 0)
	{
		i--;

		while(i >= 0 && m_Path[i] != '\\' && m_Path[i] != '/')
		{
			i--;
		}

		i++;

		if(m_Path[i] >= '0' && m_Path[i] <= '9')
		{
			m_ID = atoi(m_Path + i);
		}
	}

	// Get ranked if existing
	i = strlen(m_Path)-1;
	while(i >= 0 && m_Path[i] != '\\' && m_Path[i] != '/')
	{
		i--;
	}

	if(i > 0)
	{
		i--;

		while(i >= 0 && m_Path[i] != '\\' && m_Path[i] != '/')
		{
			i--;
		}

		if(i > 0)
		{
			i--;

			while(i >= 0 && m_Path[i] != '\\' && m_Path[i] != '/')
			{
				i--;
			}

			i++;

			if(!strncmp(&m_Path[i], "Ranked", 6))
			{
				m_Status = 'r';
			}
			else
			{
				m_Status = 's';
			}
		}
	}
	





	m_Loaded = true;


	return 1;
}


void CDiff::read(CSerializedFile& Database)
{
	// Read
	Database
		>> m_Name
		>> m_Path
		>> m_FileTime
		>> m_BID
		>> m_ID
		>> m_Status
		>> m_Mode
		>> m_HitCircles
		>> m_MaxCombo;


	m_Crc = g_CRC.CalcHash(m_Path);


	// Load difficulties
	for(u32 i = 0; i < 9; i++)
	{
		Database
			>> SpeedDifficulty[i]
			>> JumpDifficulty[i]
			>> AR[i]
			>> OD[i];
	}


	// Load other stuff if loaded, else not!
	bool loaded;
	Database >> loaded;


	if(loaded)
	{
		m_Data = new DiffData;

		Database
			>> m_Data->HPDrainRateOrig
			>> m_Data->CircleSizeOrig
			>> m_Data->OverallDifficultyOrig
			>> m_Data->ApproachRateOrig
			>> m_Data->StackLeniency
			>> m_Data->SliderMultiplier;

		// Read hitobjects
		u32 size;

		Database >> size;
		HitObject* HO;


		s16 posBuf[2];

		for(u32 i = 0; i < size; i++)
		{
			HO = new HitObject;

			Database
				>> HO->timeOrig
				>> posBuf[0]
				>> posBuf[1]
				>> HO->type;

			HO->posOrig.X = (f32)posBuf[0];
			HO->posOrig.Y = (f32)posBuf[1];


			if(HO->type == HO_SPINNER)
			{
				Database >> HO->durationOrig;
			}
			else if(HO->type == HO_SLIDER)
			{
				HO->slider = new Slider;

				Database
					>> HO->slider->timeOrig
					>> HO->slider->length
					>> HO->slider->repetitions;


#ifdef __STORE_LARP
				u32 larp_size;
				Database >> larp_size;

				for(u32 j = 0; j < larp_size; j++)
				{
					HO->larp.push_back(vector2d<f64>(0, 0));
					Database
						>> HO->larp[j].X
						>> HO->larp[j].Y;
				}
#else
				Database >> HO->slider->type;
			
				u32 points_size;
				Database >> points_size;

				for(u32 j = 0; j < points_size; j++)
				{
					HO->slider->points.push_back(vector2d<f64>(0, 0));
					Database
						>> posBuf[0]
						>> posBuf[1];

					HO->slider->points[j].X = (f64)posBuf[0];
					HO->slider->points[j].Y = (f64)posBuf[1];
				}

#endif
			}

			m_Data->HitObjects.push_back(HO);
		}


		// Read timing sections
#ifndef __STORE_LARP

		Database >> size;
		TimingPoint* TP;

		for(u32 i = 0; i < size; i++)
		{
			TP = new TimingPoint;

			Database
				>> TP->time
				>> TP->Number
				>> TP->inherited;

			m_Data->TimingPoints.push_back(TP);
		}

#endif

		m_Loaded = true;
	}
}


void CDiff::store(CSerializedFile& Database)
{
	// Store global values
	Database
		<< m_Name
		<< m_Path
		<< m_FileTime
		<< m_BID
		<< m_ID
		<< m_Status
		<< m_Mode
		<< m_HitCircles
		<< m_MaxCombo;


	// Store difficulties
	for(u32 i = 0; i < 9; i++)
	{
		Database
			<< SpeedDifficulty[i]
			<< JumpDifficulty[i]
			<< AR[i]
			<< OD[i];
	}



	// Store other stuff if loaded, else not!
	bool loaded = false;
	Database << loaded;


	if(loaded)
	{
		Database
			<< m_Data->HPDrainRateOrig
			<< m_Data->CircleSizeOrig
			<< m_Data->OverallDifficultyOrig
			<< m_Data->ApproachRateOrig
			<< m_Data->StackLeniency
			<< m_Data->SliderMultiplier;

		// Store hitobjects
		Database << m_Data->HitObjects.size();


		s16 posBuf[2];

		for(u32 i = 0; i < m_Data->HitObjects.size(); i++)
		{
			HitObject* HO = m_Data->HitObjects[i];


			posBuf[0] = (s16)HO->posOrig.X;
			posBuf[1] = (s16)HO->posOrig.Y;

			Database
				<< HO->timeOrig
				<< posBuf[0]
				<< posBuf[1]
				<< HO->type;

			if(HO->type == HO_SPINNER)
			{
				Database << HO->durationOrig;
			}
			else if(HO->type == HO_SLIDER)
			{
				// Write slider object
				Database
					<< HO->slider->timeOrig
					<< HO->slider->length
					<< HO->slider->repetitions;

				// Write larp
#ifdef __STORE_LARP
				Database << HO->larp.size();

				for(u32 j = 0; j < HO->larp.size(); j++)
				{
					Database
						<< HO->larp[j].X
						<< HO->larp[j].Y;
				}
#else

				Database
					<< HO->slider->type
					<< HO->slider->points.size();

				for(u32 j = 0; j < HO->slider->points.size(); j++)
				{
					posBuf[0] = (s16)HO->slider->points[j].X;
					posBuf[1] = (s16)HO->slider->points[j].Y;

					Database
						<< posBuf[0]
						<< posBuf[1];
				}

#endif
			}
		}


		// Store timing sections
#ifndef __STORE_LARP

		Database << m_Data->TimingPoints.size();

		for(u32 i = 0; i < m_Data->TimingPoints.size(); i++)
		{
			TimingPoint* TP = m_Data->TimingPoints[i];

			Database
				<< TP->time
				<< TP->Number
				<< TP->inherited;
		}

#endif
	}

	

}





#ifdef __SQL

void CDiff::storeDBlisting(char* query, CDatabaseConnection* pDatabase)
{

	// Do mysql stuff




	// Store each individual beatmap


	if(m_BID == 0)
	{
		__LOG(MSG_WARNING, "\"%s\" has no /b/ ID.", m_Name);
		return;
	}


	sprintf(&query[strlen(query)], "(%d,%d,'", m_BID, m_ID);



	// File
	s32 pos = 0;
	while(m_Name[pos] != ' ')
	{
		pos++;
	}
	pos++;

	pDatabase->escape(&query[strlen(query)], &m_Name[pos]);

	strcat(query, "','");


	// Name
	pDatabase->escape(&query[strlen(query)], m_Data->Name);

	strcat(query, "','");

	// Artist
	pDatabase->escape(&query[strlen(query)], m_Data->Artist);

	strcat(query, "','");

	// Creator
	pDatabase->escape(&query[strlen(query)], m_Data->Creator);

	strcat(query, "','");

	// Diffname
	pDatabase->escape(&query[strlen(query)], m_Data->Diffname);

	strcat(query, "','");

	// Source
	if(m_Data->Source != NULL)
	{
		pDatabase->escape(&query[strlen(query)], m_Data->Source);
	}

	strcat(query, "','");
	strncat(query, &m_Status, 1);
	strcat(query, "',");
	sprintf(&query[strlen(query)], "%d,%d,%d", m_MaxCombo, m_HitCircles, m_Data->HitObjects.size());
	strcat(query, "),");
	



}


void CDiff::storeDB(char* query, s32 ModValue)
{
	// Do mysql stuff




	// Store each individual beatmap
		

	if(m_BID == 0)
	{
		__LOG(MSG_WARNING, "\"%s\" has no /b/ ID.", m_Name);
		return;
	}


	sprintf(&query[strlen(query)], "(%d,%.02f,%.02f,%.02f,%.02f,%.02f),",
		m_BID,
		m_Data->ApproachRate, m_Data->CircleSize, m_Data->OverallDifficulty,
		get_SpeedDifficulty(ModValue), get_JumpDifficulty(ModValue));



}
#endif



void CDiff::init()
{
	if(!m_Loaded)
	{
		__LOG(MSG_ERROR, "Can't initialize when not loaded.");
		return;
	}


	m_Data->speedStrains.resize(m_Data->HitObjects.size());
	m_Data->jumpStrains.resize(m_Data->HitObjects.size());

	// Sort
	sort(m_Data->HitObjects.begin(), m_Data->HitObjects.end(), compareHO);


	// Pull times to beginning
	m_Data->offsetOrig = m_Data->HitObjects[0]->timeOrig;
	for(unsigned int i = 1; i < m_Data->HitObjects.size(); i++)
	{
		m_Data->HitObjects[i]->timeOrig -= (s32)m_Data->offsetOrig;
	}

	for(u32 i = 0; i < m_Data->TimingPoints.size(); i++)
	{
		m_Data->TimingPoints[i]->time -= (s32)m_Data->offsetOrig;
	}

	m_Data->HitObjects[0]->timeOrig = 0;


	// Get min and maxbpm
	m_Data->minBPMOrig = 10000;
	m_Data->maxBPMOrig = 0;
	f64 BPM = 0;
	f64 SliderMult = -100;

	TimingPoint* TP;
	for(u32 i = 0; i < m_Data->TimingPoints.size(); i++)
	{
		TP = m_Data->TimingPoints[i];


		if(TP->inherited && TP->Number < 0)
		{
			SliderMult = TP->Number;
		}
		else
		{
			SliderMult = -100;

			BPM = 60000 / TP->Number;


			if(m_Data->minBPMOrig > BPM)
			{
				m_Data->minBPMOrig = BPM;
			}

			if(m_Data->maxBPMOrig < BPM)
			{
				m_Data->maxBPMOrig = BPM;
			}
		}



		TP->BPM = BPM;
		TP->SliderMultiplier = SliderMult;


	}



	m_MaxCombo = 0;
	m_HitCircles = 0;

	Slider* slider;
	u32 j = 0;
	for(unsigned int i = 0; i < m_Data->HitObjects.size(); i++)
	{
		HitObject* HO = m_Data->HitObjects[i];


		while(j+1 < m_Data->TimingPoints.size() && HO->timeOrig >= m_Data->TimingPoints[j+1]->time)
		{
			j++;
		}


		slider = HO->slider;


		// Bezier if NOT done already
		if(slider != NULL)
		{

			if(HO->larp.empty())
			{
				switch(slider->type)
				{

				case 'P':
					if(slider->points.size() == 3)
					{
						if(slider->points[1] == slider->points[2])
						{
							HO->larp.push_back(slider->points[0]);
							HO->larp.push_back(slider->points[1]);
						}
						else
						{
							name = m_Name;
							HO->circle();
						}

					}
					else
					{
						HO->bezier();
					}
					break;

				case 'B':
					HO->bezier();
					break;

				case 'L':
				case 'C':
					// Simple linear interpolation
					for(u32 k = 0; k < slider->points.size(); k++)
					{
						HO->larp.push_back(slider->points[k]);

						if(k+1 < slider->points.size())
						{
							if(slider->points[k] == slider->points[k+1])
							{
								k++;
							}
						}
					}
					break;


				default:

					__LOG(MSG_WARNING, "Unknown slider type '%c' in \"%s\".", HO->slider->type, m_Name);
					break;
				}






				// LARP is filled! adjust time, then length!
				slider->timeOrig = (f32)(((-600.0 / m_Data->TimingPoints[j]->BPM) * (f64)slider->length * m_Data->TimingPoints[j]->SliderMultiplier) / (100.0 * m_Data->SliderMultiplier));

				

				if(HO->larp.size() <= 1)
				{
					__LOG(MSG_INFO, "LOL1 %d", HO->slider->points.size());
				}

				f64 dist = 0;
				f64 overhead;
				for(u32 k = 0; k+1 < HO->larp.size(); k++)
				{
					dist += (HO->larp[k+1] - HO->larp[k]).getLength();

					if((f32)dist > slider->length)
					{
						while(k+2 < HO->larp.size())
						{
							HO->larp.pop_back();
						}

						overhead = dist - slider->length;

						f64 newDist = (HO->larp[k+1] - HO->larp[k]).getLength();

						HO->larp[k+1] = HO->larp[k] +
							(HO->larp[k+1] - HO->larp[k]) * ((newDist - overhead) / newDist);

						break;
					}
				}

				if((f32)dist < slider->length)
				{
					if(HO->larp.size() <= 1)
					{
						__LOG(MSG_INFO, "LOL");
					}

					HO->larp.push_back(HO->larp.back() +
						((HO->larp.back() - HO->larp[HO->larp.size()-2]).normalize()) * (f64)(slider->length - (f32)dist));
				}
			}





			if(m_Data->osuFileFormatVersion <= 7)
			{
				slider->ticks = (int)floor(((slider->timeOrig / 1000) * (m_Data->TimingPoints[j]->BPM / 60) * m_Data->SliderTickRate * ((double)(-100) / m_Data->TimingPoints[j]->SliderMultiplier)) - 0.01);
			}
			else
			{
				slider->ticks = (int)floor(((slider->timeOrig / 1000) * (m_Data->TimingPoints[j]->BPM / 60) * m_Data->SliderTickRate) - 0.01);
			}

			m_MaxCombo += 1 + (slider->repetitions) * (1 + slider->ticks);

		}
		else
		{
			if(m_Data->HitObjects[i]->type != HO_SPINNER)
			{
				m_HitCircles++;
			}
			m_MaxCombo++;
		}

	}





	// Length of beatmap
	m_Data->lengthOrig = 0;
	f64 res;

	for(u32 i = 0; i < m_Data->HitObjects.size(); i++)
	{
		HitObject* HO = m_Data->HitObjects[i];

		if(HO->slider == NULL)
		{
			if(HO->type == HO_SPINNER)
			{
				res = (f64)(HO->timeOrig + HO->durationOrig);
			}
			else
			{
				res = (f64)HO->timeOrig;
			}
		}
		else
		{
			res = (f64)((f64)HO->timeOrig + (f64)HO->slider->timeOrig * HO->slider->repetitions);
		}

		if(res > m_Data->lengthOrig)
		{
			m_Data->lengthOrig = res;
		}
	}
}



f32 CDiff::stack(u32 index, f32 dist, bool slider)
{
	f32 res = 0;
	s32 depth = (s32)(m_Data->ApproachRateMsec * m_Data->StackLeniency);
	HitObject* HO = m_Data->HitObjects[index];
	HitObject* HO2;


	HO->offset.X -= dist;


	for(s32 i = index - 1; i >= 0 && HO->time - (HO2 = m_Data->HitObjects[i])->time <= depth; i--)
	{
		if(HO->type != HO_SPINNER && HO2->type != HO_SPINNER)
		{
			// Begin position is the same
			if(abs(HO->posOrig.X - HO2->posOrig.X) <= 2 && abs(HO->posOrig.Y - HO2->posOrig.Y) <= 2)
			{
				if(HO2->slider == NULL || HO->offset.X != 0 || HO->type == HO_CIRCLE)
				{
					res = stack(i, dist+1, (HO2->slider != NULL) || slider);
					break;
				}
			}
			// Slider ending shit
			else if(HO2->slider != NULL &&
				HO->posOrig.getDistanceFrom(vector2d<f32>((f32)HO2->larp.back().X, (f32)HO2->larp.back().Y)) <= 3)
			{
				if(slider == true)
				{
					res = stack(i, dist+1, true);
					break;
				}
				else
				{
					// Found slider
					res = dist;
				}

			}
		}
	}


	HO->offset.X += res;

	return res;
}



void CDiff::preprocess(s32 ModValue)
{

	bool HR, Easy, HT, DT;
	HR   = (ModValue == E_MODS_HR || ModValue == E_MODS_DT_HR || ModValue == E_MODS_HT_HR);
	Easy = (ModValue == E_MODS_EASY || ModValue == E_MODS_DT_EASY || ModValue == E_MODS_HT_EASY);
	HT   = (ModValue == E_MODS_HT || ModValue == E_MODS_HT_EASY || ModValue == E_MODS_HT_HR);
	DT   = (ModValue == E_MODS_DT || ModValue == E_MODS_DT_EASY || ModValue == E_MODS_DT_HR);


	// Parse orig values
	m_Data->HPDrainRate = m_Data->HPDrainRateOrig;
	m_Data->CircleSize = m_Data->CircleSizeOrig;
	m_Data->OverallDifficulty = m_Data->OverallDifficultyOrig;
	m_Data->ApproachRate = m_Data->ApproachRateOrig;
	m_Data->length = m_Data->lengthOrig;
	m_Data->offset = m_Data->offsetOrig;



	for(unsigned int i = 0; i < m_Data->HitObjects.size(); i++)
	{
		HitObject* HO = m_Data->HitObjects[i];

		
		HO->time = HO->timeOrig;
		HO->pos = HO->posOrig;
		HO->offset = vector2d<f32>(0, 0);
		HO->duration = HO->durationOrig;


		if(HO->slider != NULL)
		{
			HO->slider->time = HO->slider->timeOrig;
		}

	}


	m_Data->maxBPM = m_Data->maxBPMOrig;
	m_Data->minBPM = m_Data->minBPMOrig;


	if(m_Data->HitObjects.size() < 1)
		return;


	// MODS



	// HR
	if(HR)
	{
		m_Data->HPDrainRate = min(10, (double)m_Data->HPDrainRate * (double)1.4);
		m_Data->CircleSize = min(10, (double)m_Data->CircleSize * (double)1.3);
		m_Data->OverallDifficulty = min(10, (double)m_Data->OverallDifficulty * (double)1.4);
		m_Data->ApproachRate = min(10, (double)m_Data->ApproachRate * (double)1.4);
	}

	// Easy
	if(Easy)
	{
		m_Data->HPDrainRate = (double)m_Data->HPDrainRate * (double)0.5;
		m_Data->CircleSize = (double)m_Data->CircleSize * (double)0.5;
		m_Data->OverallDifficulty = (double)m_Data->OverallDifficulty * (double)0.5;
		m_Data->ApproachRate = (double)m_Data->ApproachRate * (double)0.5;
	}


	// DT
	if(DT)
	{
		for(unsigned int i = 0; i < m_Data->HitObjects.size(); i++)
		{
			HitObject* HO = m_Data->HitObjects[i];


			HO->time = (s32)(HO->time * ((double)2/3));
			HO->duration = (s32)(HO->duration * ((double)2/3));

			if(HO->slider != NULL)
			{
				HO->slider->time = (f32)(HO->slider->time * ((double)2/3));
			}
		}

		s32 armsec;

		if(m_Data->ApproachRate > 5)
		{
			armsec = (s32)(1200 - (m_Data->ApproachRate - 5) * 150);
		}
		else
		{
			armsec = (s32)(1800 - m_Data->ApproachRate * 120);
		}

		armsec = (s32)(armsec * ((double)2/3));


		if(armsec < 1200)
		{
			m_Data->ApproachRate = -((double)armsec - 1200)/(double)150 + 5;
		}
		else
		{
			m_Data->ApproachRate = -((double)armsec - 1800)/(double)120;
		}


		double odmsec = (78.0 - m_Data->OverallDifficulty * 6);
		odmsec = (odmsec * ((double)2/3));

		m_Data->OverallDifficulty = -((double)odmsec - 78)/(double)6;


		m_Data->minBPM *= 1.5;
		m_Data->maxBPM *= 1.5;


		m_Data->length *= (f64)2/3;
		m_Data->offset *= (f64)2/3;
	}

	// HT
	if(HT)
	{
		for(unsigned int i = 0; i < m_Data->HitObjects.size(); i++)
		{
			HitObject* HO = m_Data->HitObjects[i];


			HO->time = (s32)(HO->time * ((double)4 / (double)3));
			HO->duration = (s32)(HO->duration * ((double)4 / (double)3));

			if(HO->slider != NULL)
			{
				HO->slider->time = (f32)(HO->slider->time * ((double)4 / (double)3));
			}
		}

		s32 armsec;

		if(m_Data->ApproachRate > 5)
		{
			armsec = (s32)(1200 - (m_Data->ApproachRate - 5) * 150);
		}
		else
		{
			armsec = (s32)(1800 - m_Data->ApproachRate * 120);
		}
		
		armsec = (s32)(armsec * ((double)4 / (double)3));


		if(armsec < 1200)
		{
			m_Data->ApproachRate = -((double)armsec - 1200)/(double)150 + 5;
		}
		else
		{
			m_Data->ApproachRate = -((double)armsec - 1800)/(double)120;
		}



		double odmsec = (78.0 - m_Data->OverallDifficulty * 6);
		odmsec = (odmsec * ((double)4 / (double)3));

		m_Data->OverallDifficulty = -((double)odmsec - 78)/(double)6;


		m_Data->minBPM *= 0.75;
		m_Data->maxBPM *= 0.75;


		m_Data->length *= (double)4 / (double)3;
		m_Data->offset *= (double)4 / (double)3;
	}

	AR[ModValue] = m_Data->ApproachRate;
	OD[ModValue] = m_Data->OverallDifficulty;

	
	


	if(m_Data->ApproachRate > 5)
	{
		m_Data->ApproachRateMsec = (s32)(1200 - (m_Data->ApproachRate - 5) * 150);
	}
	else
	{
		m_Data->ApproachRateMsec = (s32)(1800 - m_Data->ApproachRate * 120);
	}


	// Dimension
	m_Data->CircleSizePixel = (s32)(156 - m_Data->CircleSize * 12);
	float posFactor = (float)156 / (float)m_Data->CircleSizePixel;
	m_Data->CircleSizePixel /= 4;


	// Stacking recursively

	f32 pixel = (f32)m_Data->CircleSizePixel / 7;
	for(s32 i = (s32)m_Data->HitObjects.size()-1; i >= 0; i--)
	{
		HitObject* HO = m_Data->HitObjects[i];

		if(HO->type != HO_SPINNER && HO->offset.X == 0)
		{
			stack(i, 0, HO->slider != NULL);
		}

		HO->offset.X *= pixel;

		if(HR)
		{
			HO->offset.Y = -HO->offset.X;
		}
		else
		{
			HO->offset.Y = HO->offset.X;
		}
	}


	
	HitObject* HO;
	double radius = m_Data->CircleSizePixel * 3;
	double dist;

	for(unsigned int i = 0; i < m_Data->HitObjects.size(); i++)
	{
		HO = m_Data->HitObjects[i];

		// Calc relative positions to equalize Circle Size & normalize timestamps
		HO->pos = (HO->pos + HO->offset) * posFactor;


		// Calc lazyLarp
		if(HO->slider != NULL)
		{
			HO->larpLazy.clear();

			HO->larpLazy.push_back(HO->larp.front());


			

			HO->slider->lengthLazy = 0;

			for(u32 i = 1; i < HO->larp.size(); i++)
			{
				dist = HO->larpLazy[i-1].getDistanceFrom(HO->larp[i]);

				if(dist > radius)
				{
					HO->larpLazy.push_back(HO->larpLazy[i-1] + (HO->larp[i] - HO->larpLazy[i-1]).normalize() * (dist - radius));
					HO->slider->lengthLazy += (f32)dist - (f32)radius;
				}
				else
				{
					HO->larpLazy.push_back(HO->larpLazy[i-1]);
				}
			}

			HO->slider->lengthLazy *= posFactor;


			HO->larpLazyBack.clear();

			HO->larpLazyBack.push_back(HO->larpLazy.back());

			HO->slider->lengthLazyBack = 0;

			s32 index = 0;
			for(s32 i = HO->larpLazy.size() - 2; i >= 0; i--)
			{
				dist = HO->larpLazyBack[index].getDistanceFrom(HO->larp[i]);

				if(dist > radius)
				{
					HO->larpLazyBack.push_back(HO->larpLazyBack[index] + (HO->larp[i] - HO->larpLazyBack[index]).normalize() * (dist - radius));
					HO->slider->lengthLazyBack += (f32)dist - (f32)radius;
				}
				else
				{
					HO->larpLazyBack.push_back(HO->larpLazyBack[index]);
				}

				index++;
			}

			HO->slider->lengthLazyBack *= posFactor;



		}
		
	}


	


	
	
	




	HitObject* HO0 = m_Data->HitObjects[0];
	HO0->speedStrain = 1;
	HO0->jumpStrain = 1;


	double decayS;
	double decayJ;



	// Get peak & average
	m_Data->PeakSpeedDifficulty = 1;
	m_Data->AverageSpeedDifficulty = 1;

	m_Data->PeakJumpDifficulty = 1;
	m_Data->AverageJumpDifficulty = 1;

	HitObject* HOPrev = HO0;
	vector2d<f32> End;

	for(u32 i = 1; i < m_Data->HitObjects.size(); i++)
	{
		HO = m_Data->HitObjects[i];



		double jumpAddition = 0;
		double streamAddition = 0;
		if(HO->type == HO_SPINNER)
		{
		}
		else if(HOPrev->slider != NULL)
		{
			End = (vector2d<f32>((f32)HOPrev->larpLazy.back().X, (f32)HOPrev->larpLazy.back().Y) + HOPrev->offset) * posFactor;
		

			jumpAddition = (jumpSpacingWeight(HO->pos.getDistanceFrom(End)) + jumpSpacingWeight(HOPrev->slider->lengthLazy) +
				(HOPrev->slider->repetitions - 1) * jumpSpacingWeight(HOPrev->slider->lengthLazyBack));

			streamAddition = speedSpacingWeight(HO->pos.getDistanceFrom(End) + HOPrev->slider->lengthLazy + (HOPrev->slider->repetitions - 1) * HOPrev->slider->lengthLazyBack);
		}
		else if(HOPrev->type == HO_CIRCLE)
		{
			jumpAddition = jumpSpacingWeight(HO->pos.getDistanceFrom(HOPrev->pos));
			streamAddition = speedSpacingWeight(HO->pos.getDistanceFrom(HOPrev->pos));
		}

		
		decayS = pow(__DECAY_SPEED, (double)(HO->time - HOPrev->time) / 1000);

		HO->speedStrain =
			HOPrev->speedStrain *
			decayS + (streamAddition * 1400) / (double)(max(HO->time - HOPrev->time, 50));



		if(HO->speedStrain < 0)
		{
			HO->speedStrain = 0;
		}

		// Peak and avg
		m_Data->AverageSpeedDifficulty += HO->speedStrain;
		if(HO->speedStrain > m_Data->PeakSpeedDifficulty)
		{
			m_Data->PeakSpeedDifficulty = HO->speedStrain;
		}

		// ------------------------------------------------------------------

		decayJ = pow(__DECAY_JUMP, (double)(HO->time - HOPrev->time) / 1000);

		HO->jumpStrain =
			HOPrev->jumpStrain *
			decayJ + (jumpAddition * 25 / (double)(max(HO->time - HOPrev->time, 50)));

		if(HO->jumpStrain < 0)
		{
			HO->jumpStrain = 0;
		}

		// Peak and avg
		m_Data->AverageJumpDifficulty += HO->jumpStrain;
		if(HO->jumpStrain > m_Data->PeakJumpDifficulty)
		{
			m_Data->PeakJumpDifficulty = HO->jumpStrain;
		}

		HOPrev = HO;


	}

	m_Data->AverageSpeedDifficulty /= m_Data->HitObjects.size();
	m_Data->AverageJumpDifficulty /= m_Data->HitObjects.size();
	

}







double CDiff::jumpSpacingWeight(float distance)
{
	return pow(distance, 0.99f) * 1.05;
}


double CDiff::speedSpacingWeight(float distance)
{
	if(distance > SINGLE_SPACING_TRESHOLD)
	{
		return 2.5;
	}
	else if(distance > STREAM_SPACING_TRESHOLD)
	{
		return 1.6 + 0.9 * (distance - STREAM_SPACING_TRESHOLD) / (SINGLE_SPACING_TRESHOLD - STREAM_SPACING_TRESHOLD);
	}
	else if(distance > DIAMETER)
	{
		return 1.2 + 0.4 * (distance - DIAMETER) / (STREAM_SPACING_TRESHOLD - DIAMETER);
	}
	else if(distance > DIAMETER / 2)
	{
		return 0.95 + 0.25 * (distance - (DIAMETER / 2)) / (DIAMETER / 2);
	}

	return 0.95;
}



bool comparef64(f64 f1, f64 f2)
{
	return (f1 > f2);
}












void CDiff::calcLevel(s32 ModValue)
{

	if(m_Data->HitObjects.size() < 1 || m_Mode != 0)
	{
		m_Data->PeakJumpDifficulty = 0;
		m_Data->AverageJumpDifficulty = 0;

		m_Data->PeakSpeedDifficulty = 0;
		m_Data->AverageSpeedDifficulty = 0;

		SpeedDifficulty[ModValue] = 0;
		JumpDifficulty[ModValue] = 0;

		AR[ModValue] = 0;
		OD[ModValue] = 0;

		m_Level = 0;
		return;
	}


	// Calculate strain per hitcircle
#ifdef __STRAIN

	char filename[512];
	strcpy(filename, g_Config.StrainPath);
	itoa(ModValue, &filename[strlen(filename)], 10);
	strcat(filename, "/");
	itoa(m_BID, &filename[strlen(filename)], 10);
	strcat(filename, ".dat");
	


	FILE* pFile = fopen(filename, "rb");

	if(pFile == NULL)
	{
		pFile = fopen(filename, "ab");

		
	}



	CSerializedFile StrainFile(pFile);




#endif


	preprocess(ModValue);





#ifdef __STRAIN

	StrainFile
	//	<< m_Data->ApproachRate
	//	<< m_Data->OverallDifficulty
	//	<< m_Data->CircleSize
		<< m_Data->offset
		<< m_Data->length
		<< m_Data->HitObjects.size();


	HitObject* HO;
	for(u32 i = 0; i < m_Data->HitObjects.size(); i++)
	{
		HO = m_Data->HitObjects[i];
		StrainFile
			<< HO->time
			<< HO->speedStrain
			<< HO->jumpStrain;
	}
	

	StrainFile
		<< m_Data->AverageSpeedDifficulty
		<< m_Data->AverageJumpDifficulty
		<< m_Data->PeakSpeedDifficulty
		<< m_Data->PeakJumpDifficulty;

#endif


#ifdef __DISPLAY_BPM


	SpeedDifficulty[ModValue] = m_Data->minBPM / 2;
	JumpDifficulty[ModValue] = m_Data->maxBPM / 2;


#else

	m_Data->speedStrains.clear();
	m_Data->jumpStrains.clear();


	double maxs = 0;
	double maxj = 0;
	double decayS;
	double decayJ;
	s32 step = 400;
	s32 pos = step;
	for(u32 i = 0; i < m_Data->HitObjects.size(); i++)
	{
		while(m_Data->HitObjects[i]->time > pos)
		{
			m_Data->speedStrains.push_back(maxs);
			m_Data->jumpStrains.push_back(maxj);

			decayS = pow(__DECAY_SPEED, (double)(pos - m_Data->HitObjects[i-1]->time) / 1000);
			decayJ = pow(__DECAY_JUMP, (double)(pos - m_Data->HitObjects[i-1]->time) / 1000);

			maxs = m_Data->HitObjects[i-1]->speedStrain * decayS;
			maxj = m_Data->HitObjects[i-1]->jumpStrain * decayJ;

			if(maxs < 0)
			{
				maxs = 0;
			}
			if(maxj < 0)
			{
				maxj = 0;
			}

			pos += step;
		}
		

		if(m_Data->HitObjects[i]->speedStrain > maxs)
		{
			maxs = m_Data->HitObjects[i]->speedStrain;
		}

		if(m_Data->HitObjects[i]->jumpStrain > maxj)
		{
			maxj = m_Data->HitObjects[i]->jumpStrain;
		}
	}

	m_Data->speedStrains.push_back(maxs);
	m_Data->jumpStrains.push_back(maxj);


	sort(m_Data->speedStrains.begin(), m_Data->speedStrains.begin() + m_Data->speedStrains.size(), comparef64);
	sort(m_Data->jumpStrains.begin(), m_Data->jumpStrains.begin() + m_Data->jumpStrains.size(), comparef64);


	// Get 


	SpeedDifficulty[ModValue] = 0;
	JumpDifficulty[ModValue] = 0;

	f64 factor = 1;
	f64 sqr1, sqr2;

	for(u32 i = 0; i < m_Data->speedStrains.size(); i++)
	{
		sqr1 = m_Data->speedStrains[i];
		sqr2 = m_Data->jumpStrains[i];
		SpeedDifficulty[ModValue] += sqr1 * factor;
		JumpDifficulty[ModValue] += sqr2 * factor;
		factor *= 0.90;
	}

	SpeedDifficulty[ModValue] = pow(5 * max(1, sqrt(SpeedDifficulty[ModValue])) - 4, 3) / 100000;
	JumpDifficulty[ModValue] = pow(5 * max(1, sqrt(JumpDifficulty[ModValue])) - 4, 3) / 100000;

#endif

#ifdef __STRAIN

	/*StrainFile
		<< SpeedDifficulty[ModValue]
		<< JumpDifficulty[ModValue];*/

	fclose(pFile);

#endif

}


void CDiff::applySpeedJump(s32 SpeedJump, s32 ModValue)
{
	double ratio = ((double)SpeedJump / 100);


	//m_Level = SpeedDifficulty[ModValue] + JumpDifficulty[ModValue] + abs(SpeedDifficulty[ModValue] - JumpDifficulty[ModValue]) * 0.5;
	m_Level = ((double)1 - ratio) * SpeedDifficulty[ModValue] + ratio * JumpDifficulty[ModValue];

}








void CDiff::printDetails(s32 ModValue)
{
	printf("   Speed difficulty: %.02f\r\n"
		   "   Jump difficulty:  %.02f\r\n"
		   "\r\n", SpeedDifficulty[ModValue], JumpDifficulty[ModValue]);
}


void CDiff::writeDetails(char* pText, s32 ModValue)
{
	sprintf(pText, "\r\n"
		"   Speed difficulty: %.02f\r\n"
		"   Jump difficulty:  %.02f"
		, SpeedDifficulty[ModValue], JumpDifficulty[ModValue]);
}


void CDiff::logDetails(FILE* pFile, s32 ModValue)
{
	fprintf(pFile,
		   "   Speed difficulty: %.02f\r\n"
		   "   Jump difficulty:  %.02f\r\n"
		   "\r\n", SpeedDifficulty[ModValue], JumpDifficulty[ModValue]);
}


bool compareDiff(CDiff* d1, CDiff* d2)
{
	return d1->get_Level() > d2->get_Level();
}


bool compareDiff2(CDiff* d1, CDiff* d2)
{
	if(d1->get_Crc() == d2->get_Crc())
	{
		return strcmp(d1->get_Path(), d2->get_Path()) > 0;
	}

	return d1->get_Crc() > d2->get_Crc();
}

bool compareDiff3(CDiff* d1, CDiff* d2)
{
	return d1->get_BID() > d2->get_BID();
}