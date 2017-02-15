/*
    xyza2pipe - a cross conversion environment of NMR spectra
    Copyright 2017 Masashi Yokochi

    https://github.com/yokochi47/xyza2pipe
     forked from http://fermi.pharm.hokudai.ac.jp/olivia/

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
 */

#include "xyza2pipe.h"

static const char *usage[] = {
		"vnmr2pipe -i/--in inTemplate(2D/3D/4D) > stdout\n", "Optional Arguments:\n",
		" -p/--pdir    A directory that includes VNMR procpar file. (default=\".\")\n",
		" -e/--extLeft Extract left hand of X-Axis.\n",
		" -c/--adjCAR  Adjust indirect carrier frequencies according to 1H calibration.\n",
		" -h/--adjH2O  Adjust all carrier frequencies assuming transmitter is set to H2O solvent.\n",
		" -x  Output X-Vector first\n", " -y  Output Y-Vector first\n", " -z  Output Z-Vector first\n",
		" -a  Output A-Vector first\n", " --xLAB X-Axis Label\n", " --yLAB Y-Axis Label\n", " --zLAB Z-Axis Label\n",
		" --aLAB A-Axis Label\n", " --xCAR X-Axis Center [ppm]\n", " --yCAR Y-Axis Center [ppm]\n",
		" --zCAR Z-Axis Center [ppm]\n", " --aCAR A-Axis Center [ppm]\n", " --ndim dimCount\n",
		" NOTE: User's phase correction is effective in hyper-complex 2D data.\n", " --rp  0th Order X-Phase [deg]\n",
		" --lp  1st Order X-Phase [deg]\n", " --rp1 0th Order Y-Phase [deg]\n", " --lp1 1st Order Y-Phase [deg]\n\n", ""
};

int main(int argc, char *argv[])
{
	char axis_option = 'x';
	char filename[MAXLONGNAME] = { "" };
	char pardir[MAXLONGNAME] = { "." };
	char monofile[MAXLONGNAME] = { "monofile" };
	int l, opt_idx = 0;

	static struct option options[] = {
			{"in", required_argument, 0, 'i'},
			{"pdir", required_argument, 0, 'p'},
			{"x", no_argument, 0, 'x'},
			{"y", no_argument, 0, 'y'},
			{"z", no_argument, 0, 'z'},
			{"a", no_argument, 0, 'a'},
			{"extLeft", no_argument, 0, 'e'},
			{"adjCAR", no_argument, 0, 'c'},
			{"adjH2O", no_argument, 0, 'h'},
			{"xLAB", required_argument, 0, 6},
			{"yLAB", required_argument, 0, 7},
			{"zLAB", required_argument, 0, 8},
			{"aLAB", required_argument, 0, 9},
			{"xCAR", required_argument, 0, 10},
			{"yCAR", required_argument, 0, 11},
			{"zCAR", required_argument, 0, 12},
			{"aCAR", required_argument, 0, 13},
			{"ndim", required_argument, 0, 'd'},
			{"rp", required_argument, 0, 20},
			{"rp1", required_argument, 0, 21},
			{"lp", required_argument, 0, 30},
			{"lp1", required_argument, 0, 31},
			{0}
	};

	while (1) {
		l = getopt_long(argc, argv, "i:p:xyzaechd:", options, &opt_idx);

		if (l == -1)
			break;

		switch (l) {
		case 0:
			if (options[opt_idx].flag != 0)
				break;
			fprintf(stderr, "option %s", options[opt_idx].name);
			if (optarg)
				fprintf(stderr, " with arg %s", optarg);
			fputc('\n', stderr);
			break;
		case 'i':
			if (optarg)
				strncpy(filename, optarg, MAXLONGNAME);
			break;
		case 'p':
			if (optarg)
				strncpy(pardir, optarg, MAXLONGNAME);
			break;
		case 'x':
			axis_option = 'x';
			break;
		case 'y':
			axis_option = 'y';
			break;
		case 'z':
			axis_option = 'z';
			break;
		case 'a':
			axis_option = 'a';
			break;
		case 'e':
			extleft = 1;
			break;
		case 'c':
			adjcar = 1;
			break;
		case 'h':
			adjh2o = 1;
			break;
		case 'd':
			dimension = atoi(optarg);
			break;
		case 6:
			if (optarg) {
				usrlabel = 1;
				strncpy(axislabel[0], optarg, MAXASSNAME);
			}
			break;
		case 7:
			if (optarg) {
				usrlabel = 1;
				strncpy(axislabel[1], optarg, MAXASSNAME);
			}
			break;
		case 8:
			if (optarg) {
				usrlabel = 1;
				strncpy(axislabel[2], optarg, MAXASSNAME);
			}
			break;
		case 9:
			if (optarg) {
				usrlabel = 1;
				strncpy(axislabel[3], optarg, MAXASSNAME);
			}
			break;
		case 10:
			usrshift = 1;
			usrcenter[0] = atof(optarg);
			break;
		case 11:
			usrshift = 1;
			usrcenter[1] = atof(optarg);
			break;
		case 12:
			usrshift = 1;
			usrcenter[2] = atof(optarg);
			break;
		case 13:
			usrshift = 1;
			usrcenter[3] = atof(optarg);
			break;
		case 20:
			usrphase[0] = 1;
			phase[0][0] = atof(optarg);
			break;
		case 21:
			usrphase[1] = 1;
			phase[1][0] = atof(optarg);
			break;
		case 30:
			usrphase[0] = 1;
			phase[0][1] = atof(optarg);
			break;
		case 31:
			usrphase[1] = 1;
			phase[1][1] = atof(optarg);
			break;
		case '?':			/* getopt_long already printed an error message. */
			break;
		}
	}

	if (argc == 2 && *argv[1] != '-')
		strncpy(filename, argv[1], MAXLONGNAME);

	else if (optind < argc) {
		fprintf(stderr, "non-option ARGV-elements: ");
		while (optind < argc)
			fprintf(stderr, "%s ", argv[optind++]);
		fputc('\n', stderr);

		return 1;
	}

	if (filename[0] == 0) {
		l = 0;
		while (strcmp(usage[l], "") != 0)
			fprintf(stderr, "%s", usage[l++]);

		return 1;
	}

	if (checkvnmr(filename, pardir, monofile) != 0)
		return 1;

	cnvhdr(axis_option, 'f');

	if (isatty(STDOUT_FILENO)) {
		fprintf(stderr, "vnmr2pipe error: output to terminal.\n");
		return 1;
	}

	if (adjcar != 0 && adjh2o != 0)
		fprintf(stderr, "'--adjH2O' option overrides '--adjCAR'.\n");

	switch (dimension) {
	case 2:
		return pushvnmr2d(monofile, pardir, axis_option);
		break;
	case 3:
		return pushvnmr3d(monofile, pardir, axis_option);
		break;
	case 4:
		return pushvnmr4d(monofile, pardir, axis_option);
		break;
	}

	return 0;
}
