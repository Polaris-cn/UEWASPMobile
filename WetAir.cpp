/*library UEWASP; 
ע��
     ��DLL ����IAPWS-IF97 ��IFC67
     (1985IAPS����ѧ���ʹ��ʹǼܱ�)��ʪ����
	 ����(���������ڡ�1994��11�µ����棬
	 �й�������ҵ������)��д��
     ��DLL�еĸ��������ĵ�λ���ǹ��ʵ�λ��
     ����ѹ���ĵ�λΪMPa���¶ȵĵ�λΪ�棬
	 �ʵĵ�λkJ/kg���صĵ�λkJ/kg.K��
     ˮ�������¶ȷ�Χ273.16 K~1073.15 K (0.01��~800��)
     ѹ����0~100 MPa��
	 ʪ�����¶ȷ�Χ273.16 K~473.15 K (0.01��~200��)��
     ѹ����40000Pa~400000Pa��
     
*/ 

//************************************************************
//     ��    �ߣ�                                            *
//              ��ɳ��������������޹�˾(UESoft Corp.) �����*
//     �ļ����ƣ�                                            * 
//                DLL�ⲿ�ӿ�ʵ��(ˮ������ʪ�������ʼ���)    *
//     ���ʱ�䣺                                            *
//                2006��5��                                  *
//************************************************************ 

#include "wetair.h"
#include "UEwasp.h"
#include "UEwasp67.h"
#include "UEwasp97.h"
#include <fstream>

#define MAXNUM_GAS 12
#define T0 273.15

short AIR=5;
bool WetAir_BLIF97;
double R=8.3143;//ͨ�����峣��,J/mol.K,8314.3kJ/kmol.K
double fPSA=101325E-6;//��׼����ѹ,MPa,fPSA=101325Pa
double fPLA=101325E-6;//���ش���ѹ,MPa
double fVWBSA=2.5;//ʪ������������,һ��ȡ>=2.5m/s
struct GasData mArrGasData[MAXNUM_GAS]=
{0,"H2",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
1,"O2",32.00,25.48,15.200,5.0620,1.31200,273,1800,1.19,\
2,"N2",28.02,28.90,-1.570,8.0810,-28.730,273,1800,0.59,\
3,"CO",28.01,28.16,1.6750,5.3720,-2.2220,273,1800,0.89,\
4,"CO2",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
5,"AIR",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
6,"H2O_S",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
7,"CH4",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
8,"C2H4",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
9,"C2H6",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
10,"C3H6",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
11,"C3H8",2.016,29.21,-1.916,-4.004,-0.8705,273,1800,1.01,\
};

int *RANGE;
double *H;
double *S;
double *P;
double *T;
double *V;
double *X;

 void _stdcall  WetAir_SETSTD_WetBulbSurface_AirVelocity(double fWetBulbSurface_AirVelocity)
{//ʪ������������,m/s,fWetBulbSurface_AirVelocity
   fVWBSA=fWetBulbSurface_AirVelocity;
}

 void _stdcall  WetAir_SETSTD_atmosphere_pressure(double fLocalAtmospherePressure)
{//���ش���ѹfLocalAtmospherePressure
   fPLA=fLocalAtmospherePressure;
}

 void _stdcall  WetAir_GETSTD_atmosphere_pressure(double fLocalAtmospherePressure)
{
	fLocalAtmospherePressure=fPLA;
}

 void _stdcall  WetAir_SETSTD_WASP(int  STDID)
{
     if (STDID==67)
          WetAir_BLIF97=false;
      else
          WetAir_BLIF97=true ;    
}


 void _stdcall  WetAir_GETSTD_WASP(int *STDID)
{
    if (WetAir_BLIF97==false)
        *STDID=67;
    else
        *STDID=97;
}

 short _stdcall  WetAir_GetIndex(char* cName)
{//�����������
	short i=0;
	 for( i=0;i<MAXNUM_GAS;i++)
	 {
		 if(cName==mArrGasData[i].cName)
			 return i;
	 }
	 return i;//��ʱ�������������1	 
}

  double _stdcall  WetAir_GetRva()
{//���ˮ/����������֮��Rva=0.6219
	 return WetAir_GetIndex("H2O_S")/WetAir_GetIndex("AIR");
}

  double _stdcall  WetAir_GetCp(short i,double T)
{//����������Cp
	T=T+T0;
	double fCp=mArrGasData[i].a0+mArrGasData[i].a1*1e-3*T+mArrGasData[i].a2*1e-6*T*T+mArrGasData[i].a3*1e-9*T*T*T;
	fCp=fCp/mArrGasData[i].fMolecular_Weigh;//unit:kJ/kg.K
	return fCp;
}

  double _stdcall  WetAir_GetEnthalpy(short i,double T)
{//���������ֵEnthalpy
	return WetAir_GetCp(i,T+T0)*T;//kJ/kg
}

  double _stdcall  WetAir_D2P(double d)
{//���ˮ����ѹP--unit:MPa;����ʪ��D--kg/kg�ɿ���
	return fPLA*d/(d+WetAir_GetRva());
}

  double _stdcall  WetAir_P2D(double P)
{//��þ���ʪ��D--kg/kg�ɿ���;ˮ����ѹP--unit:MPa;
	return WetAir_GetRva()*P/(fPSA-P);
}

  double _stdcall  WetAir_GetEnthalpy_wetair(double T,double W)
{//���ʪ������ֵEnthalpy--unit:kJ/kg,=�ɿ�������ha+ˮ��������hv
    if (WetAir_BLIF97==true ) 
    {
        PT2H97(WetAir_D2P(W),T,H,RANGE);
    }
    else
    {
        PT2H67(WetAir_D2P(W),T,H,RANGE);
    }
	return WetAir_GetEnthalpy(AIR,T)+W*(*H);
}

//��֪�����¶�t(��)��ʪ���¶�tw(��)����ˮ����ѹP(MPa)
 double _stdcall  WetAir_TT2P(double T, double Tw, double phi1)
 {//���¼�����������(������)��P13��1994��11�µ����棬2006��1�µ�21��ӡˢ���й�������ҵ������
	 //A=alpha/(r*beta*fPSA)
	 //���鹫ʽA=(65+6.75/v)*1e-5,һ���������v>=2.5m/s
	 //Pq=ʪ����Χ������ˮ����ѹ��,Pa
	 //Pqb1=ʪ�����ˮ���µı���ˮ����ѹ��,Pa��
	 //Pqb1Ҳ�൱��ˮ����һ�����Ϳ��������ˮ����ѹ��,Pa
	 double Ps1;//Ps1=�����¶�T��Ӧ�ı���ˮ����ѹ��,MPa
	 //phi1=������ʪ��,0~1
	double A=(65+6.75/fVWBSA)*1e-5;//in general A as 6.67e-4
	double *Pqb1=NULL;
    if ( WetAir_BLIF97==true ) 
    {
        T2P97(Tw,Pqb1,RANGE);
		T2P97(T,&Ps1,RANGE);
    }
    else
    {
        T2P67(Tw,Pqb1,RANGE);
		T2P67(T,&Ps1,RANGE);
    }
	double p=*Pqb1-A*(T-Tw)*fPLA;
	phi1=p/Ps1;
	return p;
}

 //��֪�����¶�t(��)�;��ȱ����¶�tw(��)�������ʪ��D(kg/kg.dry)
 double _stdcall  WetAir_TT2D(double T, double Tw, double phi1)
 {//���¼�����ů��ͨ�缰��������--���������(ԭ����6��)��P43��2005��5�µ�1�棬2005��5�µ�1��ӡˢ����ѧ��ҵ������
	 //�����ȶ�����״̬���г������غ㷽�̣������:
	 //W1*(iv1-iw)=(ia2-ia1)+Ws2*(iv2-iw)
	 //W1=���ȱ��������ʪ��������ʪ��,kg/kg��
	 //Ws2=���ȱ��������ڱ���ʪ��������ʪ��,kg/kg��
	 //ia1=���ȱ�������ڸɿ�����,kJ/kg
	 //ia2=���ȱ��������ڸɿ�����,kJ/kg
	 double iv1;//iv1=���ȱ��������ˮ������,kJ/kg
	 double iv2;//iv2=���ȱ���������ˮ������,kJ/kg
	 double iw;//iw=���ȱ�������Һ��ˮ��,kJ/kg
	 double Pv2;//Pv2=ʪ�����ˮ���µı���ˮ����ѹ��,MPa
	 //Pv2 Ҳ�൱��ˮ����һ�����Ϳ��������ˮ����ѹ��,MPa
	 double Pv1;//Pv1=���ȱ��������ˮ����ѹ��,MPa
	 double Ps1;//Ps1=���ȱ���������¶�T��Ӧ�ı���ˮ����ѹ��,MPa
	 //phi1=���ȱ�����������ʪ��,0~1
	 double fTmp=0;
	Pv1=WetAir_TT2P(T,Tw,fTmp);
	
    if ( WetAir_BLIF97==true ) 
    {
		PT2H97(Pv1,T,&iv1,RANGE);
        T2HL97(Tw,&iw,RANGE);
		T2HG97(Tw,&iv2,RANGE);
		T2P97(Tw,&Pv2,RANGE);
		T2P97(T,&Ps1,RANGE);
    }
    else
    {
		PT2H67(Pv1,T,&iv1,RANGE);
        T2HL67(Tw,&iw,RANGE);
		T2HG67(Tw,&iv2,RANGE);
		T2P67(Tw,&Pv2,RANGE);
		T2P67(T,&Ps1,RANGE);
    }
	phi1=Pv1/Ps1;//Relative Humidity(0~1)
	return (WetAir_GetEnthalpy(AIR,Tw)-WetAir_GetEnthalpy(AIR,T)+WetAir_P2D(Pv2)*(iv2-iw))/(iv1-iw);
}

