#include "fertigation.h"

//ˮ�ʻ�����
/*static union { //��վ��ַ0x51
    float set_float[28];//ˮ�ʻ��ܵ����ü�PID�趨
    u8 set_int[28][4];
} fertigation51;
static u8 set_fertigation51_finish[28];
static u8 read_fertigation_finish51;
static void chge_fertisnd(u8 flg);
static union { //��վ��ַ0x52
    float prarm_float[6];//ˮ�ʻ��ܵ����ü�PID�趨
    u8 param_int[6][4];
} fertigation52;

param_fertigation52[38] ��¼0x52�豸�·��������� ��������
[0][1] ʩ�ʴ���
[2][3] ʩˮ����
[4][5] ������Ԥ��
[6][7] Ԥ��ʱ��
[8][9]  ���趨ʩ��ʱ��
[10][11] ÿĶʩ������ֵ
[12][13]  ��������ϴ
[14][15]  ��ϴʱ��
[16][17]  һ��ʩ�ʹ��
[18][19]  ����ʩ�࿪ʼ
[20][21]  �ι������л�
[22][23]  �����趨
[24][25]   �������1
[26][27]   �������2
[28][29]   �������3
[30][31]   ����ʩ��1
[32][33]   ����ʩ��2
[34][35]   ����ʩ��3
[36][37]   ����ʩ�ʱ�־

�������ݶ��ǵ�λ��ǰ����λ�ں�
*/
u8 reportfertigation52[3][16]={0};
u8 fertigation52databuf[12] = { 0 }; //���ڿ�52�·��ĸ�������ת��2�ֽ�
u8 SF_cmd_num[28][2] = { 0 }; //����������Ϣ���ʹ����������ջظ�ʱ�����±괦��0,��ά�����ʾ��·������
u8 SF_Qeury_index=0;      //��ѯ�·����յ��Ļظ�ȷ��
u8 SF_Trans_flg=0;					//�����·���־λ������·�������·��趨����
u16 SF_collector_temp = 0;
u8 SF_wgcollector_data_buff[16] = {0};
u8 SF_TD_param_num = 0;
#define SF_Para_Tran_time 500

#define TXENABLE3		         GPIO_SetBits(GPIOD, GPIO_Pin_10)

u8 SF_USART3SendTCB[128]={0};
u8 SF_Flow_Trans_Flg = 0;      //�����·���־λ������·�������·���������Լ��ܹ�����
u16 SF_Flow_Total[6] = {0};  //ȡ6��˲ʱ�ܹ�������ƽ��ֵ�·�
u8 SF_Flow_Total_I = 0;
u8 SF_Flow_Total_flg = 0;
u8 SF_Lvbo641=12;
u8 SF_Lvbo642=12;
u8 SF_Lvbo646=12;

extern union //��վ��ַ0x51
{
      float set_float[28];//ˮ�ʻ��ܵ����ü�PID�趨
      u8 set_int[28][4];
}fertigation51;
 extern u8 set_fertigation51_finish[28];
 extern u8 read_fertigation_finish51;

 extern union //��վ��ַ0x52
{
      float prarm_float[6];//ˮ�ʻ��ܵ����ü�PID�趨
      u8 param_int[6][4];
}fertigation52;
 extern u8 param_fertigation52[38];
 extern u8 fertigation_flg;

extern  u8 USART3SendTCB[128];
extern u8 factory_gateway_set[255];
extern u8 bytelen3;
extern  u8 ReportData3[128];
extern u8 freq_I;
extern u8  wgcollector_data_buff[16];
extern u16  TIM2_FrequencyPC0[61];
extern u16  TIM2_FrequencyPA1[61];
extern u16 TIM2_FrequencyPA0[61];
extern u16 temp_adc;
extern u8  WriteMultipleRegister(u8 Slave_ID,u16 addr,u16 num,u8 *pData,u8 *temp);
extern void WriteDataToDMA_BufferTX3(uint16_t size);
extern u16 Get_Adclvbo(u8 TD_Xnumber,u16 TD_xiaxian,u16 TD_shangxian);
extern u8 First_adc_average[3];
extern u16 First_Getaverage(u8 td_xnumber,u8 maxlvbo_xnumber,u16 temp_adc);
static const u8 Maxlvbo_number= 50;
static const u8 MaxTD_number= 3;	
extern u8 TDlvbo_number[MaxTD_number];
extern u8 tdcycle_i[MaxTD_number];
extern u16 TD_Getaverage(u8 td_xnumber,u8 tdlvbo_xnumber,u16 temp_xadc,u8 tdcycle_xi);
extern u8 Collectors[65][16];
extern u8 TxFlag3 ;
void SF_Para_Trans() { //���յ���0x51ˮ��PID�����·�����վ��ַΪSF_SlaveID_0��������
	
	
    switch(SF_Trans_flg) {
    case 0:
        while (fertigation51.set_float[SF_Qeury_index] == 0) {

            SF_Qeury_index++;
            if (SF_Qeury_index >= 28) {
                SF_Qeury_index = 0;
                return;
            }

        }

        if (SF_cmd_num[SF_Qeury_index][0] != 0) {
            bytelen3 = WriteMultipleRegister(SF_SlaveID_0, SF_Qeury_index + 40, 2, fertigation51.set_int[SF_Qeury_index], ReportData3);
            memcpy(USART3SendTCB, ReportData3, bytelen3);  
            WriteDataToDMA_BufferTX3(bytelen3);
            SF_cmd_num[SF_Qeury_index][0]--;
						SF_Trans_flg=0;
						Start_timerEx(SF_Para_Trans_EVT,200);
						return;
        }
        SF_Trans_flg=1;
        Start_timerEx(SF_Para_Trans_EVT,400);
        break;

    case 1:
        if (SF_cmd_num[SF_Qeury_index][1] != 0) {
            bytelen3 = WriteMultipleRegister(SF_SlaveID_1, SF_Qeury_index + 40, 2, fertigation51.set_int[SF_Qeury_index], ReportData3);
            memcpy(USART3SendTCB, ReportData3, bytelen3);
            WriteDataToDMA_BufferTX3(bytelen3);
            SF_cmd_num[SF_Qeury_index][1]--;
						SF_Trans_flg=1;
						Start_timerEx(SF_Para_Trans_EVT,200);
						return;
        }
       
				if(SF_cmd_num[SF_Qeury_index][1] == 0 && SF_cmd_num[SF_Qeury_index][0] == 0)
				{
						SF_Qeury_index++;
						if (SF_Qeury_index >= 28) {
							SF_Trans_flg =0;
							SF_Qeury_index = 0;
							return;
						}
				}
			  SF_Trans_flg =0;
        Start_timerEx(SF_Para_Trans_EVT,400);
        break;
    default:
        break;
    }
}

  void SF_Flow_Measu() { //��·����������
    
    if(factory_gateway_set[12]==33 ||factory_gateway_set[15]==33 ||factory_gateway_set[24]==33) {
        SF_TD_param_num = 0;
        if(factory_gateway_set[12]==33) { //ˮ����������������  //��һ·��������·���վ pc0 H0900
            u8 i;
            SF_collector_temp=0;
            for(i=freq_I+1; i<=SF_Lvbo641; i++) {
                SF_collector_temp=SF_collector_temp+TIM2_FrequencyPC0[i];
            }
            for(i=0; i<freq_I; i++) {
                SF_collector_temp=SF_collector_temp+TIM2_FrequencyPC0[i];   //collector_tempΪ6s����������1L������Ϊ450��
            }
            SF_collector_temp=80*SF_collector_temp/SF_Lvbo641;//����1λС����������Ҫ����10����collector_temp*3600*10/450/lvbo641���������˲�12s
            SF_wgcollector_data_buff[SF_TD_param_num]   =  SF_collector_temp & 0x00ff;           //ʩ�������ֽڵ�
            wgcollector_data_buff[0] = SF_wgcollector_data_buff[SF_TD_param_num];
            SF_TD_param_num++;
            SF_wgcollector_data_buff[SF_TD_param_num]   =  (SF_collector_temp & 0xff00)>>8;     //ʩ�������ֽڸߣ�
            wgcollector_data_buff[1] = SF_wgcollector_data_buff[SF_TD_param_num];
            SF_TD_param_num++;
        } else {
            SF_TD_param_num++;
            SF_TD_param_num++;
        }

        if(factory_gateway_set[15]==33) { //ˮ���������������� �ڶ�·��������·���վ pa1 H0904
            u8 i;
            SF_collector_temp=0;
            for(i=freq_I+1; i<=SF_Lvbo641; i++) {
                SF_collector_temp=SF_collector_temp+TIM2_FrequencyPA1[i];
            }
            for(i=0; i<freq_I; i++) {
                SF_collector_temp=SF_collector_temp+TIM2_FrequencyPA1[i];   //collector_tempΪ6s����������1L������Ϊ450��
            }
            SF_collector_temp=80*SF_collector_temp/SF_Lvbo641;//����1λС����������Ҫ����10����collector_temp*3600*10/450/12���������˲�12s
            SF_wgcollector_data_buff[SF_TD_param_num]   =  SF_collector_temp & 0x00ff;           //ʩ�������ֽڵ�
            wgcollector_data_buff[2] = SF_wgcollector_data_buff[SF_TD_param_num];
            SF_TD_param_num++;
            SF_wgcollector_data_buff[SF_TD_param_num]   =  (SF_collector_temp & 0xff00)>>8;     //ʩ�������ֽڸߣ�
            wgcollector_data_buff[3] = SF_wgcollector_data_buff[SF_TD_param_num];
            SF_TD_param_num++;

        } else {
            SF_TD_param_num++;
            SF_TD_param_num++;
        }
				
			if (factory_gateway_set[21] == 3 || factory_gateway_set[21] == 8) {
            SF_collector_temp=0;
            if (factory_gateway_set[21] == 8) {
                temp_adc = Get_Adclvbo(1, 0, 1305);
            } else {
                temp_adc = Get_Adclvbo(1, 262, 1305);
            }
            if (First_adc_average[1] == 0) {
                temp_adc = First_Getaverage(1, Maxlvbo_number, temp_adc);
                First_adc_average[1] = 1;
            }
            TDlvbo_number[1] = factory_gateway_set[12 + 3 * 3 + 2];//ȡ���������趨���˲��������ֽڣ��˲��������255��
            temp_adc = TD_Getaverage(1, TDlvbo_number[1], temp_adc, tdcycle_i[1]);
            First_adc_average[1] = 1;
            tdcycle_i[1]++;
            if (tdcycle_i[1] >= TDlvbo_number[1]) {
                tdcycle_i[1] = 0;
            }
            SF_collector_temp = temp_adc; //�����¶�
            wgcollector_data_buff[14] = SF_collector_temp & 0x00ff;          	//�����¶ȵ�λ
            wgcollector_data_buff[15] = (SF_collector_temp & 0xff00) >> 8;    	//�����¶ȸ�λ
        }

        if(factory_gateway_set[24]==33) { //ˮ���������������� �ڶ�·��������·���վ pa0 H0914
            u8 i;
            SF_collector_temp=0;
            for(i=freq_I+1; i<=SF_Lvbo641; i++) {
                SF_collector_temp=SF_collector_temp+TIM2_FrequencyPA0[i];
            }
            for(i=0; i<freq_I; i++) {
                SF_collector_temp=SF_collector_temp+TIM2_FrequencyPA0[i];   //collector_tempΪ6s����������1L������Ϊ450��
            }
            SF_collector_temp=80*SF_collector_temp/SF_Lvbo641;//����1λС����������Ҫ����10����collector_temp*3600*10/450/12���������˲�12s
            SF_wgcollector_data_buff[SF_TD_param_num]   =  SF_collector_temp & 0x00ff;           //ʩ�������ֽڵ�
            wgcollector_data_buff[10] = SF_wgcollector_data_buff[SF_TD_param_num];
            SF_TD_param_num++;
            SF_wgcollector_data_buff[SF_TD_param_num]   =  (SF_collector_temp & 0xff00)>>8;     //ʩ�������ֽڸߣ�
            wgcollector_data_buff[11] = SF_wgcollector_data_buff[SF_TD_param_num];
            SF_TD_param_num++;

        } else {
            SF_TD_param_num++;
            SF_TD_param_num++;
        }
        /*wgcollector_data_buff[16]
        				[0][1] �ɼ���������641   ��һ·����
        				[2][3] �ɼ���������642   �ڶ�·����
        				[4][5] �ɼ���������643
        				[6][7] �ɼ���������644
        				[8][9] �ɼ���������645
        				[10][11] �ɼ���������646   ����·����
        				[12][13] �ɼ���������647		�ܹ�����   ����5����
        				[14][15] �ɼ���������648   �ܹ�ѹ�� 4-20mA�ź� data1��ͷ
        			*/
        memcpy(Collectors[64], wgcollector_data_buff, 16);
        SF_wgcollector_data_buff[8]=(u16)((fertigation52.prarm_float[3] * 100)) & 0x00FF;    //��һ·ʩ��Ũ�ȷŴ�100�������ֽ�
        SF_wgcollector_data_buff[9]=(u16)((fertigation52.prarm_float[3] * 100))>>8;         //��һ·ʩ��Ũ�ȷŴ�100�������ֽ�
        SF_wgcollector_data_buff[10]=(u16)((fertigation52.prarm_float[4] * 100)) & 0x00FF;  //�ڶ�·ʩ��Ũ�ȷŴ�100�������ֽ�
        SF_wgcollector_data_buff[11]=(u16)((fertigation52.prarm_float[4] * 100))>>8;         //�ڶ�·ʩ��Ũ�ȷŴ�100�������ֽ�
        SF_wgcollector_data_buff[12]=(u16)((fertigation52.prarm_float[5] * 100)) & 0x00FF;  //����·ʩ��Ũ�ȷŴ�100�������ֽ�
        SF_wgcollector_data_buff[13]=(u16)((fertigation52.prarm_float[5] * 100))>>8;        //����·ʩ��Ũ�ȷŴ�100�������ֽ�
        SF_wgcollector_data_buff[14]=param_fertigation52[36] | (param_fertigation52[36]<<8);        //����ʩ�ʱ�־ =1�������ʼʩ�� =0����� û��ʩ��
        /*SF_wgcollector_data_buff[16]
        				[0][1]   ��һ·����
        				[2][3]   �ڶ�·����
        				[4][5]   ����·����
        				[6][7]   �ܹ�����
        				[8][9]   ��һ·�趨Ũ��
        				[10][11] �ڶ�·�趨Ũ��
        				[12][13] ����·�趨Ũ��
        				[14]      ����ʩ�ʱ�־
        				[15]
        			*/

        Start_timerEx(SF_Flow_Trans_EVT,50);
    }
}
void SF_WriteDataToDMA_BufferTX3(uint16_t size) {
    TXENABLE3;
    TxFlag3 = 1;
    DMA1_Channel2->CMAR = (uint32_t)SF_USART3SendTCB;
    DMA1_Channel2->CNDTR = (uint16_t)size; // ����Ҫ���͵��ֽ���Ŀ
    USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);  // ��������DMA����
    DMA_Cmd(DMA1_Channel2, ENABLE);        //��ʼDMA����
}
void SF_Flow_Trans() { //�����·ֱ��˲ʱ�����·����ܹ��������������485,�Ĵ�����ַ�̶�0x64
    switch(SF_Flow_Trans_Flg) {
    case 0:
        Start_timerEx(SF_Flow_Trans_EVT,200);
        bytelen3 = WriteMultipleRegister(SF_SlaveID_0, 100, 8, SF_wgcollector_data_buff, ReportData3); //�Ĵ���100���ڴ洢��·��������
        memcpy(SF_USART3SendTCB, ReportData3, bytelen3);
        SF_WriteDataToDMA_BufferTX3(bytelen3);
        SF_Flow_Trans_Flg++;

        break;
    case 1:
        bytelen3 = WriteMultipleRegister(SF_SlaveID_1, 100, 8, SF_wgcollector_data_buff, ReportData3);
        memcpy(SF_USART3SendTCB, ReportData3, bytelen3);
        SF_WriteDataToDMA_BufferTX3(bytelen3);
        SF_Flow_Trans_Flg = 0;
        break;
    default:
        break;
    }

}


