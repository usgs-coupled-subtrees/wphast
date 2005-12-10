#pragma once

class CPrintInput
{
public:
	CPrintInput(void);
	~CPrintInput(void);
public:
	void SyncWithSrcInput(void);
public:
	int media;
	int conductances;
	int bc;
	int fluid;
	int method;
	int wells;
	int xyz_wells;
	// int xy;  only used for read_grid

	int force_chem;
	int comp;
	int xyz_comp;
	int head;
	int xyz_head;
	int ss_vel;
	int ss_vel_defined;
	int xyz_ss_vel;
	int xyz_ss_vel_defined;
	int hdf_chem;
	int hdf_head;
	int hdf_ss_vel;
	int hdf_ss_vel_defined;
	int xyz_chem;

	// pr
	int echo_input;
};
