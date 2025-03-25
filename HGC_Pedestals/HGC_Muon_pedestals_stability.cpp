#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TLorentzVector.h"
#include "Rtypes.h"
#include "TCanvas.h"
#include "TStyle.h"


int main ( int argc, char ** argv ){

	if ( argc == 1 ){
		std::cerr << "usage: " << argv[0] << " <input file name> <number of input files>" << std::endl ;
		return 1;
	}

	char HGC_run_name[100];

	const int HGC_run_number = atoi(argv[2]);

	std::cout << "there are " << HGC_run_number << " files to plot\n";

	char HGC_run_filename[100];
	char HGC_output_filename[100];
	
	sprintf(HGC_output_filename, "%s.txt", argv[1]);
	FILE * HGC_output_file = fopen (HGC_output_filename, "w");

	TFile * HGC_file;

	TTree * HGC_Events;

	UShort_t HGCDigi_ADC[1665] ;


	sprintf(HGC_output_filename, "%s.root", argv[1]);
	TFile * HGC_output_root_file = TFile::Open(HGC_output_filename, "RECREATE");

	Double_t HGCDigi_ADC_tot_mean[6][222];
	Double_t HGCDigi_ADC_tot_STDev[6][222];
	Double_t HGCDigi_ADC_LuB_mean[6][222][HGC_run_number];
	Double_t HGCDigi_ADC_LuB_STDev[6][222][HGC_run_number];
	Double_t HGCDigi_ADC_diff_mean[6][222][HGC_run_number];

	TH1F * HGCDigi_ADC_LuB_Hist[6][222];
	TH1F * HGCDigi_ADC_tot_Hist[6][222];

	char Hist_name[100], Hist_title[100];

	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 222; j++){
			sprintf(Hist_name, "module %d channel %d", i, j);
			sprintf(Hist_title, "module %d channel %d", i, j);
			HGCDigi_ADC_tot_Hist[i][j] = new TH1F(Hist_name, Hist_title, 2000, 0, 2000);
		}
	}

	//std::cout << "TH1F created" << std::endl;
	for (int l = 1; l < HGC_run_number+1; l++){
			
		sprintf(HGC_run_filename, "%s%d.root", argv[1], l);

		HGC_file = TFile::Open(HGC_run_filename);
   			
		HGC_Events = (TTree *) HGC_file -> Get("Events");

		HGC_Events -> SetBranchAddress("HGCDigi_adc", HGCDigi_ADC);
		HGC_Events -> SetBranchStatus("*", 0);
		HGC_Events -> SetBranchStatus("HGCDigi_adc", 1);

		std::cout << "File " << l << " Branch set" << std::endl;

		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 222; j++){
				sprintf(Hist_name, "module %d channel %d", i, j);
				sprintf(Hist_title, "module %d channel %d", i, j);
				HGCDigi_ADC_LuB_Hist[i][j] = new TH1F(Hist_name, Hist_title, 2000, 0, 2000);
			}
		}

		int HGC_Entries = HGC_Events -> GetEntries();
	
		for (int i = 0; i < HGC_Entries; i++){
			HGC_Events -> GetEntry(i);
			for (int j = 0; j < 6; j++){
				for (int k = 0; k < 222; k++){
					HGCDigi_ADC_tot_Hist[j][k] -> Fill( HGCDigi_ADC[j*222+k]);
					HGCDigi_ADC_LuB_Hist[j][k] -> Fill( HGCDigi_ADC[j*222+k]);
				}
			}
		}

		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 222; j++){
				HGCDigi_ADC_LuB_mean[i][j][l-1] = HGCDigi_ADC_LuB_Hist[i][j] -> GetMean();
				HGCDigi_ADC_LuB_STDev[i][j][l-1] = HGCDigi_ADC_LuB_Hist[i][j] -> GetStdDev();
			}
		}
	}

	std::cout << "start get means" << std::endl;

	bool is_0 = false;


	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 222; j++){
			//std::cout << HGCDigi_ADC_tot_Hist[i][j] -> GetMean() << std::endl;
			HGCDigi_ADC_tot_mean[i][j] = HGCDigi_ADC_LuB_Hist[i][j] -> GetMean();
			HGCDigi_ADC_tot_STDev[i][j] = HGCDigi_ADC_LuB_Hist[i][j] -> GetStdDev();
			//std::cout << HGCDigi_ADC_tot_mean[i][j] << " ";
			fprintf(HGC_output_file, "%.4lf ",HGCDigi_ADC_tot_Hist[i][j] -> GetMean());
		}
		//std::cout << std::endl;
		fprintf(HGC_output_file, "\n");
	}

	Double_t run_number_arr[HGC_run_number];

	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 222; j++){
			for (int k = 0; k < HGC_run_number; k++){
				HGCDigi_ADC_diff_mean[i][j][k] = HGCDigi_ADC_LuB_mean[i][j][k] - HGCDigi_ADC_tot_mean[i][j];
			}
		}
	}
	
	for (int i = 0; i < HGC_run_number; i++){
		run_number_arr[i] = i+1;
	}

	HGC_output_root_file -> cd();

	TGraphErrors* HGCDigi_ADC_Graph[6];
	TCanvas* HGCDigi_ADC_Canvas = new TCanvas("c1", "Six Graphs", 800, 600);
	HGCDigi_ADC_Canvas -> Divide(2, 3);

	for (int i = 0; i < 6; i++){
		HGCDigi_ADC_Canvas -> cd(i + 1);
		HGCDigi_ADC_Graph[i] = new TGraphErrors(HGC_run_number, run_number_arr, HGCDigi_ADC_diff_mean[0][i], nullptr, HGCDigi_ADC_LuB_STDev[0][i]);
		HGCDigi_ADC_Graph[i] -> Draw("AP");
		HGCDigi_ADC_Graph[i] -> Write();

	}

	HGCDigi_ADC_Canvas -> Write();
	HGCDigi_ADC_Canvas -> SaveAs("test.png");

	HGC_file -> Close();


	fclose(HGC_output_file);
	return 0;
}
