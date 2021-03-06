#include "FM_FTRL_machine.h"

namespace alg{

using namespace std;

FM_FTRL_machine::FM_FTRL_machine()
{
    n = NULL;
    z = NULL;
    w = NULL;

    n_fm = NULL;
    w_fm = NULL;
    z_fm = NULL;
    fm_sum = NULL;
    p_gauss_distribution = NULL;
}

FM_FTRL_machine::FM_FTRL_machine(uint32_t fm_dim,uint32_t fea_num)
{
    n = NULL;
    z = NULL;
    w = NULL;

    n_fm = NULL;
    w_fm = NULL;
    z_fm = NULL;
    fm_sum = NULL;
    p_gauss_distribution = NULL; 
    
    this->fm_dim = fm_dim;
    this->fea_num = fea_num;
}

FM_FTRL_machine::FM_FTRL_machine(uint32_t fm_dim, double fm_initDev,double L1,double L2,
		double L1_fm, double L2_fm,uint32_t fea_num,double alpha,double beta,
		double alpha_fm,double beta_fm)
{
	this->fm_dim = fm_dim;
    this->fm_initDev = fm_initDev;
    this->L1 = L1;
    this->L2 = L2;
    this->L1_fm = L1_fm;
    this->L2_fm = L2_fm;

    this->fea_num = fea_num;
        
    this->alpha = alpha;
    this->beta = beta;
    this->alpha_fm = alpha_fm;
    this->beta_fm = beta_fm;
            
/*    n = new double[fea_num];
    z = new double[fea_num];
    w = new double[fea_num];

    n_fm = new double*[fea_num];
    w_fm = new double*[fea_num];
    z_fm = new double*[fea_num];
    fm_sum = new double*[fea_num];

    for(uint32_t i = 0;i < fea_num;i++)
    {
        n_fm[i] = new double[fm_dim];
        w_fm[i] = new double[fm_dim];
        z_fm[i] = new double[fm_dim];
        fm_sum[i] = new double[fm_dim];
    }
*/
    n = (double *)calloc(fea_num,sizeof(double));
    z = (double *)calloc(fea_num,sizeof(double));
    w = (double *)calloc(fea_num,sizeof(double));

    n_fm = (double **)calloc(fea_num,sizeof(double *));
    w_fm = (double **)calloc(fea_num,sizeof(double *));
    z_fm = (double **)calloc(fea_num,sizeof(double *));
    fm_sum = (double **)calloc(fea_num,sizeof(double *));

    for(uint32_t i = 0;i < fea_num;i++)
    {
        n_fm[i] = (double *)calloc(fm_dim,sizeof(double));
        w_fm[i] = (double *)calloc(fm_dim,sizeof(double));
        z_fm[i] = (double *)calloc(fm_dim,sizeof(double));
        fm_sum[i] = (double *)calloc(fm_dim,sizeof(double));
    }

    p_gauss_distribution = new std::normal_distribution<double>(0.0,fm_initDev);

    for(uint32_t index = 0;index < fea_num;index++){
        for(uint32_t k = 0; k < this->fm_dim;  k++){
            this->z_fm[index][k] = (*p_gauss_distribution)(generator);
        }
    }
}

FM_FTRL_machine::~FM_FTRL_machine()
{
    if(n != NULL)
    {
        free(n);
        n = NULL;
    }

    if(z != NULL)
    {
        free(z);
        z = NULL;
    }

    if(w != NULL)
    {
        free(w);
        w = NULL;
    }

    if(n_fm != NULL)
    {
        for(uint32_t i = 0;i < fea_num;i++)
        {
            free(n_fm[i]);
        }
        free(n_fm);
        n_fm = NULL;
    }

    if(w_fm != NULL)
    {
        for(uint32_t i = 0;i < fea_num;i++)
        {
            free(w_fm[i]);
        }
        free(w_fm);
        w_fm = NULL;
    }

    if(z_fm != NULL)
    {
        for(uint32_t i = 0;i < fea_num;i++)
        {
            free(z_fm[i]);
        }
        free(z_fm);
        z_fm = NULL;
    }

    if(fm_sum != NULL)
    {
        for(uint32_t i = 0;i < fea_num;i++)
        {
            free(fm_sum[i]);
        }
        free(fm_sum);
        fm_sum = NULL;
    }

    if(p_gauss_distribution){
        delete p_gauss_distribution;
        p_gauss_distribution = NULL;
    }

}

/*
    fm_dim fm_initDev L1 L2 L1_fm L2_fm fea_num alpha beta alpha_fm beta_fm;
*/
/*
void FM_machine::save_model(const char *model_file,const char *ftrl_para_file)
{
    std::ofstream of_s_model(model_file);
    std::ofstream of_s_ftrl_para(ftrl_para_file);

    of_s_ftrl_para << fm_dim << " " << fm_initDev << " " << L1 << " "
         << L2 << " " << L1_fm << " " << L2_fm << " " << fea_num << " " 
         << alpha << " " << beta << " " << alpha_fm << " " 
         << beta_fm << std::endl;

    for(uint32_t i = 0;i < fea_num;i++)
    {
        of_s_ftrl_para << n[i] << " " << z[i] << " ";
        of_s_model << w[i] << " ";
        for(uint32_t j = 0; j < fm_dim;j++)
        {
            of_s_ftrl_para << n_fm[i][j] << " " << z_fm[i][j] << " " << fm_sum[i][j] << " ";
            of_s_model << w_fm[i][j] << " ";
        }
        of_s_ftrl_para << std::endl;
        of_s_model << std::endl;
    }

    of_s_ftrl_para.close();
    of_s_model.close();
 }
*/
void FM_FTRL_machine::save_model(const char *model_file,const char *ftrl_para_file)
{
    FILE *model_fp,*ftrl_para_fp;
    
    model_fp = fopen(model_file,"w");
    if(model_fp == NULL)
    {
        std::cout << "open model file error." << std::endl;
        return;
    }
    ftrl_para_fp = fopen(ftrl_para_file,"w");
    if(ftrl_para_fp == NULL)
    {
        std::cout << "open ftrl parameter file error." << endl;
        return;
    }
    /*
        model format: fm_dim fea_num
        ftrl para format: fm_initDev L1 L2 L1_fm L2_fm alpha beta alpha_fm beta_fm;
    */
    fprintf(model_fp,"%u %u\n",fm_dim,fea_num);
    fprintf(ftrl_para_fp,"%lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
            fm_initDev,L1,L2,L1_fm,L2_fm,alpha,beta,alpha_fm,beta_fm);

    /*
        ftrl parameter format: n z n_fm[0] z_fm[0] fm_sum[0] n_fm[1] z_fm[1] fm_sum[1] ... n_fm[k] z_fm[k] fm_sum[k]
        model format : w w_fm[0] w_fm[1] ... w_fm[k]
    */
    for(uint32_t i = 0;i < fea_num;i++)
    {
        fprintf(ftrl_para_fp,"%lf %lf ",n[i],z[i]);
        fprintf(model_fp,"%lf ",w[i]);
        for(uint32_t j = 0;j < fm_dim;j++)
        {
            fprintf(ftrl_para_fp,"%lf %lf %lf ",n_fm[i][j],z_fm[i][j],fm_sum[i][j]);
            fprintf(model_fp,"%lf ",w_fm[i][j]);
        }
        fprintf(ftrl_para_fp,"\n");
        fprintf(model_fp,"\n");
    }
    fclose(model_fp);
    fclose(ftrl_para_fp);
}
/*
bool FM_machine::load_model(const char *model_file,const char *ftrl_para_file)
{
    std::ifstream if_s_model(model_file);
    std::ifstream if_s_ftrl_para(ftrl_para_file);

    if_s_ftrl_para >> fm_dim >> fm_initDev >> L1 >> L2 >> L1_fm >> L2_fm >> fea_num >> alpha
        >> beta >> alpha_fm >> beta_fm;

    if(fea_num < 0 || fm_dim < 0)
        return false;

    n = new double[fea_num];
    z = new double[fea_num];
    w = new double[fea_num];

    n_fm = new double*[fea_num];
    w_fm = new double*[fea_num];
    z_fm = new double*[fea_num];
    fm_sum = new double*[fea_num];

    for(uint32_t i = 0;i < fea_num;i++)
    {
        n_fm[i] = new double[fm_dim];
        w_fm[i] = new double[fm_dim];
        z_fm[i] = new double[fm_dim];
        fm_sum[i] = new double[fm_dim];
    }

    for (uint32_t i = 0; i < fea_num; ++i)
    {
        if_s_ftrl_para >> n[i] >> z[i];
        if_s_model >> w[i];
        for(uint32_t j = 0;j < fm_dim;j++)
        {
            if_s_ftrl_para >> n_fm[i][j] >> z_fm[i][j] >> fm_sum[i][j];
            if_s_model >> w_fm[i][j];
        }
    }
    if_s_ftrl_para.close();
    if_s_model.close();

    return true;
}
*/
bool FM_FTRL_machine::load_model(const char *model_file,const char *ftrl_para_file)
{
    FILE *model_fp,*ftrl_para_fp;
    
    model_fp = fopen(model_file,"r");
    if(model_fp == NULL)
    {
        std::cerr << "open model file error." << std::endl;
        return false;
    }
    ftrl_para_fp = fopen(ftrl_para_file,"r");
    if(ftrl_para_fp == NULL)
    {
        std::cerr << "open ftrl parameter file error." << endl;
        return false;
    }
    /*
        model format: fm_dim fm_initDev L1 L2 L1_fm L2_fm fea_num alpha beta alpha_fm beta_fm;
    */
    fscanf(model_fp,"%u %u",&fm_dim,&fea_num);
    fscanf(ftrl_para_fp,"%lf %lf %lf %lf %lf %lf %lf %lf %lf",
            &fm_initDev,&L1,&L2,&L1_fm,&L2_fm,&alpha,&beta,&alpha_fm,&beta_fm);

    /*
        ftrl parameter format: n z n_fm[0] z_fm[0] fm_sum[0] n_fm[1] z_fm[1] fm_sum[1] ... n_fm[k] z_fm[k] fm_sum[k]
        model format : w w_fm[0] w_fm[1] ... w_fm[k]
    */
    for(uint32_t i = 0;i < fea_num;i++)
    {
        fscanf(ftrl_para_fp,"%lf %lf",&n[i],&z[i]);
        fscanf(model_fp,"%lf",&w[i]);
        for(uint32_t j = 0;j < fm_dim;j++)
        {
            fscanf(ftrl_para_fp,"%lf %lf %lf",&n_fm[i][j],&z_fm[i][j],&fm_sum[i][j]);
            fscanf(model_fp,"%lf",&w_fm[i][j]);
        }
    }
    fclose(model_fp);
    fclose(ftrl_para_fp);
    return true;
}
/*
bool FM_machine::load_model(const char *model_file)
{
    std::ifstream if_s_model(model_file);

    if(fea_num < 0 || fm_dim < 0)
        return false;

    w = new double[fea_num];
    w_fm = new double*[fea_num];

    for(uint32_t i = 0;i < fea_num;i++)
    {
        w_fm[i] = new double[fm_dim];
    }

    for (uint32_t i = 0; i < fea_num; ++i)
    {
        if_s_model >> w[i];
        for(uint32_t j = 0;j < fm_dim;j++)
        {
            if_s_model >> w_fm[i][j];
        }
    }
    if_s_model.close();

    return true;
}*/
bool FM_FTRL_machine::load_model(const char *model_file)
{
    FILE *model_fp;
    
    model_fp = fopen(model_file,"r");

    if(model_fp == NULL)
    {
        std::cerr << "load model file error." << std::endl;
        return false;
    }

    if(fea_num < 0 || fm_dim < 0){
        std::cerr << "error fea_num."<< std::endl;
        return false;
    }
    fscanf(model_fp,"%u %u",&fm_dim,&fea_num);

    w = (double *)calloc(fea_num,sizeof(double));
    w_fm = (double **)calloc(fea_num,sizeof(double *));

    for(uint32_t i = 0;i < fea_num;i++)
    {
        w_fm[i] = (double *)calloc(fm_dim,sizeof(double));
    }

    for (uint32_t i = 0; i < fea_num; ++i)
    {
        fscanf(model_fp,"%lf",&w[i]);
        for(uint32_t j = 0;j < fm_dim;j++)
        {
            fscanf(model_fp,"%lf",&w_fm[i][j]);
        }
    }

    fclose(model_fp);
    return true;
}

double FM_FTRL_machine::score(vector<uint32_t>fea_list,double bias)
{
    //vector<uint32_t>& fea_list = _instance.fea_vec;
    double inner = w[0];
    vector<uint32_t>::const_iterator iter = fea_list.begin();
    
    while(iter != fea_list.end()){
        if(fea_num < *iter){
            std::cerr << "error feature index " << fea_num <<" "<< *iter << std::endl;
            return 0;
        }
        inner += w[*iter];
        ++iter;
    }

    for(uint32_t i = 0 ; i< fea_list.size();i++){
        uint32_t x = fea_list[i];
        for(uint32_t j = i +1; j < fea_list.size();j++){
            uint32_t y = fea_list[j];
            for(uint32_t k = 0; k < this->fm_dim; k++){
                inner += w_fm[x][k] * w_fm[y][k];
            }
        }
    }

    inner += bias;
    return 1 / (1 + exp(-inner));
}

double FM_FTRL_machine::predict_raw(std::vector<uint32_t> fea_vec)
{
	w[0] = this->z[0]*(-1)/((this->beta + sqrt(this->n[0]))/this->alpha);
    double raw_y = w[0];


    for(size_t i = 0;i < fea_vec.size();i++){
    	uint32_t fea = fea_vec[i];
    	double sign = (z[fea] < 0 ? -1.:1.);
        if(sign * z[fea] < L1){
            w[fea] = 0.0;
        }else{
            w[fea] = (sign * L1 - z[fea])/((beta + sqrt(n[fea]))/alpha + L2);
        }
        raw_y += this->w[fea];
    }

    for(size_t i = 0;i < fea_vec.size();i++){
        uint32_t fea = fea_vec[i];
        //this->init_fm(fea);
        for(size_t k = 0; k < this->fm_dim;k++){
            //cout << "init w_fm " << fea << " k=" << k << " " << endl;
            double sign = (z_fm[fea][k] < 0 ? -1.:1.);
            if(sign * z_fm[fea][k] <= L1_fm){
                //cout << "fm is too small " << end
                w_fm[fea][k] = 0.;
            }else{
                //w_fm[fea][k] = (sign * L1_fm - z_fm[i][k]) / ((beta_fm + sqrt(n_fm[i][k])) / alpha_fm + L2_fm)
                w_fm[fea][k] = (sign * L1_fm - z_fm[fea][k]) / ((beta_fm + sqrt(n_fm[fea][k]))/alpha_fm + L2_fm);
            }
        }
            
    }

    for(size_t i = 0 ;i < fea_vec.size();i++){
        uint32_t fea_x = fea_vec[i];
        for(size_t j = i+1; j< fea_vec.size();j++){
            uint32_t fea_y = fea_vec[j];
            for(size_t k =0; k < fm_dim;k++){
                raw_y += w_fm[fea_x][k] * w_fm[fea_y][k];
            }    
        }    
    }

    return raw_y;
}

double FM_FTRL_machine::predict(std::vector<uint32_t> fea_vec)
{
	return 1./(1. + exp((-1)* max(min(this->predict_raw(fea_vec),35.),-35.)) );
}

int FM_FTRL_machine::update(std::vector<uint32_t> fea_vec,double p,double y)
{
	time_b time0;
    ftime(&time0);
    //fm sum不需清零
    //this->fm_sum.clear();
    //std::vector<double>emtpy_vec;
    //this->fm_sum.resize(D+1,emtpy_vec);
    time_b time1;
    ftime(&time1);

    double g = p - y;

    double sigma = 0.0;
    for(size_t i = 0; i < fea_vec.size();i++){
        uint32_t fea = fea_vec[i];
        sigma = (sqrt(n[fea] + g * g) - sqrt(n[fea])) / alpha; 
        z[fea] += g - sigma * w[fea];
        n[fea] += g * g;
    }
    sigma = (sqrt(n[0] + g * g) - sqrt(n[0])) / alpha;
    z[0] += g - sigma * w[0];
    n[0] += g * g;
    time_b time2;
    ftime(&time2);


    for(size_t i = 0; i < fea_vec.size(); i++){
        uint32_t fea_x = fea_vec[i];
        //if(fm_sum[fea_x].empty()){
        //    fm_sum[fea_x].resize(fm_dim,0.0);
        //}
        for(size_t j = 0; j < fea_vec.size();j++){
            uint32_t fea_y = fea_vec[j];
            //if(fm_sum[fea_y].empty()){
            //    fm_sum[fea_y].resize(fm_dim,0.0);
            //}
            if(i != j){
                for(size_t k = 0; k < fm_dim;k++){
                    fm_sum[fea_x][k] += w_fm[fea_y][k];
                }    
            }
        }    
    }
    time_b time3;
    ftime(&time3);
    for(size_t i = 0; i < fea_vec.size(); i++){ 
        uint32_t fea = fea_vec[i];
        for(size_t k = 0; k < fm_dim; k++){
            double g_fm = g * fm_sum[fea][k];
            sigma = (sqrt(n_fm[fea][k] + g_fm * g_fm) - sqrt(n_fm[fea][k])) / alpha_fm;
            z_fm[fea][k] += g_fm - sigma * w_fm[fea][k];
            n_fm[fea][k] += g_fm * g_fm;
        }
    }
    time_b time4;
    ftime(&time4);
   	//cout << "parse_time:" << get_diff(time1,time0) << " predict_time:" <<  get_diff(time2,time1)
    //                    << " update_time:" << get_diff(time3,time2)<<  
    //                " last update: " << get_diff(time4,time3) << endl;
    return 0;
}

}
