#include <iostream>
#include <iterator>
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
#include "TPaveText.h"


int main ( int argc, char ** argv ){

	if ( argc == 1 ){
		std::cerr << "usage: " << argv[0] << " <input file name> <number of input files> <output filename (no need .root) > <picture dir path (no need fo /)>" << std::endl ;
		return 1;
	}

	char HGC_run_name[100];

	const int HGC_run_number = atoi(argv[2]);

	std::cout << "there are " << HGC_run_number << " files to plot\n";

	char HGC_run_filename[100];
	char HGC_output_filename[100];
	
	sprintf(HGC_output_filename, "%s_mean.txt", argv[3]);
	FILE * HGC_output_mean_file = fopen (HGC_output_filename, "w");
	sprintf(HGC_output_filename, "%s_std.txt", argv[3]);
	FILE * HGC_output_std_file = fopen (HGC_output_filename, "w");
	//FILE * HGC_output_std1_file = fopen ("std1.txt", "w");
	//FILE * HGC_output_std2_file = fopen ("std2.txt", "w");

	TFile * HGC_file;

	TTree * HGC_Events;

	UShort_t HGCDigi_ADC[1665] ;



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
				sprintf(Hist_name, "file %d module %d channel %d", l, i, j);
				sprintf(Hist_title, "file %d module %d channel %d", l, i, j);
				HGCDigi_ADC_LuB_Hist[i][j] = new TH1F(Hist_name, Hist_title, 2000, 0, 2000);
			}
		}

		int HGC_Entries = HGC_Events -> GetEntries();
	
		for (int i = 0; i < HGC_Entries; i++){
			HGC_Events -> GetEntry(i);
			for (int j = 0; j < 6; j++){
				for (int k = 0; k < 222; k++){
					HGCDigi_ADC_tot_Hist[j][k] -> Fill(HGCDigi_ADC[j*222+k]);
					HGCDigi_ADC_LuB_Hist[j][k] -> Fill(HGCDigi_ADC[j*222+k]);
				}
			}
		}

		for (int i = 0; i < 6; i++){
			for (int j = 0; j < 222; j++){
				HGCDigi_ADC_LuB_mean[i][j][l-1] = HGCDigi_ADC_LuB_Hist[i][j] -> GetMean();
				HGCDigi_ADC_LuB_STDev[i][j][l-1] = HGCDigi_ADC_LuB_Hist[i][j] -> GetStdDev();
				//fprintf(HGC_output_std1_file, "%d %d %d %.4lf\n", l, i, j, HGCDigi_ADC_LuB_STDev[i][j][l-1]);
				delete HGCDigi_ADC_LuB_Hist[i][j];

			}
		}
		HGC_file -> Close();
	}

	std::cout << "start get means" << std::endl;

	bool is_0 = false;


	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 222; j++){
			//std::cout << HGCDigi_ADC_tot_Hist[i][j] -> GetMean() << std::endl;
			HGCDigi_ADC_tot_mean[i][j] = HGCDigi_ADC_tot_Hist[i][j] -> GetMean();
			HGCDigi_ADC_tot_STDev[i][j] = HGCDigi_ADC_tot_Hist[i][j] -> GetStdDev();
			//std::cout << HGCDigi_ADC_tot_mean[i][j] << " ";
			fprintf(HGC_output_mean_file, "%.4lf ",HGCDigi_ADC_tot_mean[i][j]);
			fprintf(HGC_output_std_file, "%.4lf ",HGCDigi_ADC_tot_STDev[i][j]);
			delete HGCDigi_ADC_tot_Hist[i][j];
		}
		//std::cout << std::endl;
		fprintf(HGC_output_mean_file, "\n");
		fprintf(HGC_output_std_file, "\n");
	}

	Double_t run_number_arr[HGC_run_number];
	TH1F * HGCDigi_ADC_diff_mean_Hist;
	Double_t HGCDigi_ADC_diff_STDev[6][222];

	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 222; j++){
			HGCDigi_ADC_diff_mean_Hist = new TH1F( "HDCDigi_ADC_diff", "HGCDigi_ADc_diff", 4000, -200, 200);	
			for (int k = 0; k < HGC_run_number; k++){
				HGCDigi_ADC_diff_mean[i][j][k] = HGCDigi_ADC_LuB_mean[i][j][k] - HGCDigi_ADC_tot_mean[i][j];
				HGCDigi_ADC_diff_mean_Hist -> Fill(HGCDigi_ADC_diff_mean[i][j][k]);
//				if (HGCDigi_ADC_LuB_STDev[i][j][k] < 30){
//					std::cout << i << " " << j << std::endl;
//				}
			}
			HGCDigi_ADC_diff_STDev[i][j] = HGCDigi_ADC_diff_mean_Hist -> GetStdDev();
			delete HGCDigi_ADC_diff_mean_Hist;
		}
	}
	
	for (int i = 0; i < HGC_run_number; i++){
		run_number_arr[i] = i+1;
	}

	sprintf(HGC_output_filename, "%s.root", argv[3]);
	TFile * HGC_output_root_file = TFile::Open(HGC_output_filename, "RECREATE");

	TGraphErrors* HGCDigi_ADC_Graph[6];
	TCanvas* HGCDigi_ADC_Canvas = new TCanvas("c1", "Six Graphs", 1600, 1200);
	HGCDigi_ADC_Canvas -> Divide(2, 3);
	HGCDigi_ADC_Canvas -> SetGrid();

	for (int i = 0; i < 6; i++){
		HGCDigi_ADC_Graph[i] = new TGraphErrors();
	}
	char HGC_Graph_title[100];
	char HGC_Graph_dir_path[100];

	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 37; j++){
			//std::cout << j * 6<< "\n";
			for (int k = 0; k < 6; k++){
				sprintf(HGC_Graph_title, "module %d channel %d", i, j*6+k);
				HGCDigi_ADC_Canvas -> cd(k + 1);
				HGCDigi_ADC_Graph[k] -> Set(HGC_run_number);
				for ( int l = 0; l < HGC_run_number; l++){
					//std::cout << "setting " << l << std::endl;
					HGCDigi_ADC_Graph[k] -> SetPoint(l, l+1, HGCDigi_ADC_diff_mean[i][j*6+k][l]);
					HGCDigi_ADC_Graph[k] -> SetPointError(l, 0, HGCDigi_ADC_LuB_STDev[i][j*6+k][l]);
					//std::cout << HGCDigi_ADC_LuB_STDev[i][j*6+k][l] << std::endl;
					//fprintf(HGC_output_std2_file, "%d %d %d %.4lf\n", l, i, j*6+k, HGCDigi_ADC_LuB_STDev[i][j*6+k][l]);
				}
				HGCDigi_ADC_Graph[k] -> SetMarkerColor(kBlue);
   				HGCDigi_ADC_Graph[k] ->	SetMarkerStyle(21);
				HGCDigi_ADC_Graph[k] -> SetLineColor(kBlue);
				HGCDigi_ADC_Graph[k] -> SetTitle(HGC_Graph_title);
				HGCDigi_ADC_Graph[k] -> Draw("ALP");
				HGCDigi_ADC_Graph[k] -> Write();

				TPaveText* pt = new TPaveText(0.60, 0.82, 0.90, 0.92, "NDC");
				pt->AddText(Form("STDev = %.2f", HGCDigi_ADC_diff_STDev[i][j*6+k]));
				pt->SetFillColor(0);
				pt->SetTextColor(HGCDigi_ADC_Graph[k]->GetMarkerColor());
				pt->Draw();
			}
			
			sprintf(HGC_Graph_dir_path, "%s/%s module %d channel %d to %d.png", argv[4], argv[3], i, j*6, j*6+5);
			HGCDigi_ADC_Canvas -> Write();
			HGCDigi_ADC_Canvas -> SaveAs(HGC_Graph_dir_path);
		}

	}
	HGC_output_root_file -> Close();

	fclose(HGC_output_file);
	std::cout << "finished\n";
	return 0;
}

