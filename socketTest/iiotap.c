#include "iiotap.h"
#include <string.h>

#define TEST_NUM 2

uint8_t MSG_FROM_SERVER[] = { 0x07, 0x55 ,0x1c ,0x00 ,0x04 ,0xf3 ,0x01 ,0x03 ,0x00 ,0xf3,0x01,0x02,0x06, 
0xe7 ,0x94 ,0xb5 ,0xe5 ,0x8e, 0x8b ,0x06 ,0x00 ,0x00, 0x00 ,0x01 ,0x05 ,0x76 ,0x61 ,0x6c ,0x75 ,0x65,
0x0a, 0x3f ,0x80 ,0x00 ,0x00 };

//输出uint8_t
void dump_mem(uint8_t* mem, uint16_t length)
{	
	uint8_t i = 0;
	uint8_t tmp;
	for (; i < length; i++) {
		tmp = mem[i] >> 4;
		if (tmp < 10) {
			putchar(tmp + '0');
		}
		else {
			putchar(tmp - 10 + 'A');
		}
		tmp = mem[i] & 0x0f;
		if (tmp < 10) {
			putchar(tmp + '0');
		}
		else {
			putchar(tmp - 10 + 'A');
		}
		putchar(32);
	}

	putchar('\n');
}


//输出char型
void getpacketchar(iiot_packet *packet)
{
	uint8_t idx = 0;
	while (idx<packet->size)
	{

		printf("%c", packet->data[idx++]);
		printf(" ");
	}
	printf("\n");
}


//编码Vint函数
uint16_t getVintencode(vint * Vnum)
{
	uint32_t tempValue = Vnum->value;
	uint8_t idx=0;
	if(tempValue !=0)
	{
		while(tempValue>0)
		{
			Vnum->encode[idx]= tempValue & 0x7F;
			tempValue = tempValue >>7;
			if(tempValue>0)
			{
				Vnum->encode[idx]= Vnum->encode[idx] | 128;		
			}	
			idx++;
		}
		Vnum->encodesize=idx;
	}
	else
	{
		Vnum->encode[0]=0x00;
		Vnum->encodesize=1;
	}
	return Vnum->encodesize;
}

//编码Ushort函数
void getUshortencode(ushort * value)
{
	uint8_t temp;
	value->ecodecont[1]= value->ushort_value & 0xff;
	temp = value->ushort_value >> 8;
	value->ecodecont[0]=temp & 0xff;
	value->ushortEncodesize=2;
}

//得到string 编码
uint16_t getStrencode(str_msg* msg,char * temp){
	uint32_t idx=0;
	uint32_t t=0;
	//字符串的长度值付给value
	
	msg->strlen_vint.value=strlen(temp);	
	//将改长度编码成vint
	msg->msgEncodsize = getVintencode(&(msg->strlen_vint));
	while (idx < msg->strlen_vint.encodesize)
	{
		msg->ecodecont[idx] = msg->strlen_vint.encode[idx];
		idx++;
	}
	while(temp[t]!='\0'){
		msg->ecodecont[idx]=temp[t];
		idx++;
		t++;
	}
	msg->msgEncodsize+=t;
	return msg->msgEncodsize;
}
//b5e7d1b9   d1  cf
//得到int编码
void getintEncode(intEn *value)
{
	uint32_t temp = value->int_value;
	value->intEncode[3] = temp & 0xff;
	value->intEncode[2] = temp>>8 & 0xff;
	value->intEncode[1] = temp>>16 & 0xff;
	value->intEncode[0] = temp>>24 & 0xff;
	value->EncodeSize = 4;
}

//编码IEEE_float编码
uint8_t getIEEE_floatencode(ieee_float_union* union_value, ieee_float * value)
{
	value->ieee_float_value = 0;
	for (int i = 0; i < 4; i++)
	{
		value->ieee_float_value |= (uint32_t)union_value->encode[i] << (i * 8);
	}
	uint32_t temp = value->ieee_float_value;
	value->encode[3] = temp & 0xff;
	value->encode[2] = temp >> 8 & 0xff;
	value->encode[1] = temp >> 16 & 0xff;
	value->encode[0] = temp >> 24 & 0xff;
	value->ieee_floatEncodesize = 4;
	return value->ieee_floatEncodesize;
}

//编码IEEE_double编码
uint8_t getIEEE_doubleencode(ieee_double_union * union_value, ieee_double * value)
{
	value->ieee_double_value = 0;
	for (int i = 0; i < 8; i++)
	{
		value->ieee_double_value |= (uint64_t)union_value->encode[i] << (i * 8);
	}
	uint64_t temp = value->ieee_double_value;
	value->encode[7] = temp & 0xff;
	value->encode[6] = temp >> 8 & 0xff;
	value->encode[5] = temp >> 16 & 0xff;
	value->encode[4] = temp >> 24 & 0xff;
	value->encode[3] = temp >> 32 & 0xff;
	value->encode[2] = temp >> 40 & 0xff;
	value->encode[1] = temp >> 48 & 0xff;
	value->encode[0] = temp >> 56 & 0xff;
	value->ieee_doubleEncodesize = 8;
	return value->ieee_doubleEncodesize;
}
//转成UTC时间
void convert_to_utc(datetime* t)
{
	if (t->hour >= 8)
	{
		t->hour -= 8;
		return;
	}
	if (t->hour < 8)
	{
		t->hour = 24 - (8 - t->hour);
		if (t->day >= 2)
		{
			t->day--;
			return;
		}
		if (t->day < 2)
		{
			if (t->month == 2 || 4 || 6 || 8 || 9 || 11)
				t->day = 31;
			if (t->month == 5 || 7 || 10 || 12)
				t->day = 30;
			if (t->month == 1)
			{
				t->day = 31;
				t->month = 12;
				t->year.ushort_value--;
			}
			if (t->month == 3)
			{
				if (t->year.ushort_value % 4 == 0 && t->year.ushort_value % 100 != 0 || t->year.ushort_value % 400 == 0)
				{
					t->day = 29;
					t->month--;
				}
				else
				{
					t->day = 28;
					t->month--;
				}
			}
		}
	}
}

//得到时间编码
uint16_t getTimeEncode(datetime* t, uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t min, uint8_t s, uint8_t ms)
{
	uint8_t i=0;
	t->year.ushort_value = y;
	t->month = m;	
	t->day = d;	
	t->hour = h;	
	t->minute = min;
	t->second = s;
	t->milisecond = ms;	
	convert_to_utc(t);
	getUshortencode(&(t->year));
	t->encode[2] = t->month;
	t->encode[3] = t->day;
	t->encode[4] = t->hour;
	t->encode[5] = t->minute;
	t->encode[6] = t->second;
	t->encode[7] = t->milisecond;
	while (i < t->year.ushortEncodesize)
	{
		t->encode[i] = t->year.ecodecont[i++];
	}
	t->encodesize = 8;
	return t->encodesize;
}



//处理头部
int assemble_header(Iiotap_Head* header,uint8_t type, uint32_t paylength,uint16_t msg_id,uint32_t DeviceAddress )
{
	//长度首先HL  1字节
	header->Length=1;	
	//T_Code是类型 1字节
	header->T_Code=type;
	header->Length++;
	//head中的payLenth值，和DeviceAddress
	header->PayLength_vint.value=paylength;
	header->DeviceAddress_vint.value=DeviceAddress;
	//payLength和DeviceAddress编码
	header->Length += getVintencode(&(header->PayLength_vint));
	header->Length += getVintencode(&(header->DeviceAddress_vint));
	/*
	dump_mem(header->PayLength_vint.encode,header->PayLength_vint.encodesize);
	dump_mem(header->DeviceAddress_vint.encode,header->DeviceAddress_vint.encodesize);
	*/
	
	if (header->T_Code > 0x3f)
	{
		header->Msg_Serial.ushort_value = msg_id;
		getUshortencode(&(header->Msg_Serial));
		header->Length = header->Length + header->Msg_Serial.ushortEncodesize;
	}
	return 0;
}

//处理注册消息体部分
void assemble_reg_payLoad(Iiotap_reg_payLoad * reg,char *username,char *password)
{	
	char *iotap="IIOTAP";	
	char *gatewayVendor="";
	char *gatewayModel="";
	char *gatewaySerial="aikefa_gateway";
	char *gatewayHardwareVersion="";
	char *gatewaySoftwareVersion="";
	reg->registerFLag=0x00;
	reg->protocolVersion=0x01;
	reg->payloadlength=2;
	reg->payloadlength+=getStrencode(&(reg->protocolName),iotap);
	reg->payloadlength+=getStrencode(&(reg->username),username);
	reg->payloadlength+=getStrencode(&(reg->password),password);
	reg->payloadlength+=getStrencode(&(reg->gatewayVendor),gatewayVendor);
	reg->payloadlength+=getStrencode(&(reg->gatewayModel),gatewayModel);
	reg->payloadlength+=getStrencode(&(reg->gatewaySerial),gatewaySerial);
	reg->payloadlength+=getStrencode(&(reg->gatewayHardwareVersion),gatewayHardwareVersion);
	reg->payloadlength+=getStrencode(&(reg->gatewaySoftwareVersion),gatewaySoftwareVersion);
}
//处理查询响应
void assemble_DeviceListResp_payLoad(Iiotap_DeviceListResp_payLoad *DeviceResp,uint16_t deviceNum,uint8_t *status,uint32_t * deviceAddr)
{
	uint8_t idx=0;
	DeviceResp->payloadlength=0;
	//设备数量，转成vint型
	DeviceResp->deviceNumber.value=deviceNum;
	DeviceResp->payloadlength += getVintencode(&(DeviceResp->deviceNumber));
	//status数量
	DeviceResp->statusSize=(deviceNum+7)/8;
	while(idx<DeviceResp->statusSize)
	{
		DeviceResp->status[idx]=status[idx];
		idx++;
	}
	DeviceResp->payloadlength+=DeviceResp->statusSize;

	idx=0;
	while(idx<deviceNum)
	{
		//设备地址
		DeviceResp->deviceAddr[idx].value=deviceAddr[idx];
		DeviceResp->payloadlength += getVintencode(&(DeviceResp->deviceAddr[idx]));
		idx++;
	}
}

//上报设备状态
void assemble_ReportDeviceStatusNotif_payLoad(Iiotap_ReportDeviceStatusNotif_payLoad * RepDevStat, uint16_t deviceNum, uint8_t *status, uint32_t * deviceAddr)
{
	uint8_t idx = 0; 
	RepDevStat->deviceNumber.value = deviceNum;
	RepDevStat->payloadlength = 0;
	RepDevStat->payloadlength+= getVintencode(&(RepDevStat->deviceNumber));
	while (idx < deviceNum)
	{
		RepDevStat->status[idx] = status[idx];
		RepDevStat->payloadlength += 1;
		//vint编码
		RepDevStat->deviceAddress[idx].value = deviceAddr[idx];
		
		RepDevStat->payloadlength += getVintencode(&(RepDevStat->deviceAddress[idx]));
		
		idx++;
	}

}


//设置上报数据的内容
void assemble_ReportDataNotif_payLoad(Iiotap_ReportDataNotif_payLoad * ReportData, uint8_t reportFlag,  uint16_t dataNum)
{
	uint8_t t = 0;
	//采集任务编码，不用，必须为0
	ReportData->taskCode = 0x00;
	ReportData->payloadlength = 1;
	//安装标签/值格式上报
	ReportData->reportFlag = reportFlag;
	ReportData->payloadlength++;
	/*使用string 表示时间
	ReportData->payloadlength+=getStrencode(&(ReportData->timeStamp), timeStamp);*/
	ReportData->payloadlength += getTimeEncode(&(ReportData->timeStamp),2017,7,18,13,50,50,55);
	//数据数量
	ReportData->dataNumber.value = dataNum;		
	ReportData->payloadlength += getVintencode(&(ReportData->dataNumber));
	while(t < dataNum)
	{
		ReportData->payloadlength += ReportData->data[t++].DataSize;
	}
}


//设置批量上报数据
void assemble_Batch_ReportDataNotif_payLoad(Iiotap_Batch_ReportDataNotif_payLoad *BatchRepData, uint8_t reportFlag,uint32_t DevNum)
{
	uint8_t t = 0;
	//采集任务编码，不用，必须为0
	BatchRepData->taskCode = 0x00;
	BatchRepData->payloadlength = 1;
	//安装标签/值格式上报
	BatchRepData->reportFlag = reportFlag;
	BatchRepData->payloadlength++;
	//时间戳
	BatchRepData->payloadlength += getTimeEncode(&(BatchRepData->timeStamp), 2017, 7, 14, 17, 50, 50, 55);
	//设备数量
	BatchRepData->deviceNumber.value = DevNum;
	BatchRepData->payloadlength += getVintencode(&(BatchRepData->deviceNumber));
	while (t < DevNum)
		BatchRepData->payloadlength += BatchRepData->device[t++].DevEncodeSize;

}

//拼接头部
void joint_header(iiot_packet * packet,Iiotap_Head* header)
{
	uint8_t idax,ivintx;		//两个下标 data的下标，vint变量的下标
	packet->data[0]=header->Length;
	packet->data[1]=header->T_Code;
	idax=2;ivintx=0;
	while(ivintx<header->PayLength_vint.encodesize)
	{
		packet->data[idax]=header->PayLength_vint.encode[ivintx];
		idax++;
		ivintx++;
	}
	if (header->T_Code>0x3f) {
		//Msg_serial
		ivintx = 0;
		while (ivintx < header->Msg_Serial.ushortEncodesize)
		{
			packet->data[idax++] = header->Msg_Serial.ecodecont[ivintx++];
		}
	}
	ivintx=0;
	while(ivintx<header->DeviceAddress_vint.encodesize)
	{
		packet->data[idax]=header->DeviceAddress_vint.encode[ivintx];
		idax++;
		ivintx++;
	}
	packet->size=idax;
	//没有msg__serial
	printf("header: \n");
	dump_mem(packet->data,packet->size);

}
//拼接注册部分
void joint_reg_body(iiot_packet * packet,Iiotap_reg_payLoad* reg)
{
	uint8_t idax,ivintx;
	idax=packet->size;	
	//1注册位
	packet->data[idax++]=reg->registerFLag;
	//2存入protocolNanem"IIOTAP"
	ivintx=0;
	while(ivintx<reg->protocolName.msgEncodsize)
	{
		packet->data[idax++]=reg->protocolName.ecodecont[ivintx++];
	}
	//3protocolVersion
	packet->data[idax++]=reg->protocolVersion;
	//4存入username
	ivintx=0;
	while(ivintx<reg->username.msgEncodsize)
	{
		packet->data[idax++]=reg->username.ecodecont[ivintx++];
	}
	//5存入password
	ivintx=0;
	while(ivintx<reg->password.msgEncodsize)
	{
		packet->data[idax++]=reg->password.ecodecont[ivintx++];
	}
	//6存入gatewayVendor
	ivintx=0;
	while(ivintx<reg->gatewayVendor.msgEncodsize)
	{
		packet->data[idax++]=reg->gatewayVendor.ecodecont[ivintx++];
	}

	//7存入gatewayModel
	ivintx=0;
	while(ivintx<reg->gatewayModel.msgEncodsize)
	{
		packet->data[idax++]=reg->gatewayModel.ecodecont[ivintx++];
	}

	//8存入gatewaySerial
	ivintx=0;
	while(ivintx<reg->gatewaySerial.msgEncodsize)
	{
		packet->data[idax++]=reg->gatewaySerial.ecodecont[ivintx++];
	}
	//9存入gatewayHardwareVersion
	ivintx=0;
	while(ivintx<reg->gatewayHardwareVersion.msgEncodsize)
	{
		packet->data[idax++]=reg->gatewayHardwareVersion.ecodecont[ivintx++];
	}
	//10存入gatewaySoftwareVersion
	ivintx=0;
	while(ivintx<reg->gatewaySoftwareVersion.msgEncodsize)
	{
		packet->data[idax++]=reg->gatewaySoftwareVersion.ecodecont[ivintx++];
	}
	packet->size=idax;
	printf("register:\n");
	dump_mem(packet->data,packet->size);

}
//拼接注册响应部分
void joint_DeviceListResp_body(iiot_packet * packet,Iiotap_DeviceListResp_payLoad * DeviceListResp)
{	
	uint8_t idax,ivintx,temp,i,numidx;
	idax=packet->size;	
	//拼接设备数量
	ivintx=0;
	while(ivintx<DeviceListResp->deviceNumber.encodesize)
	{
		packet->data[idax++]=DeviceListResp->deviceNumber.encode[ivintx++];
	}
	//拼接在线设备
	ivintx = 0; numidx = 0;
	while(ivintx<DeviceListResp->statusSize)
	{
		//设备在线状态编码
		packet->data[idax++] = DeviceListResp->status[ivintx++];
		//设备地址编码，8个设备地址
		temp=0;
		while(temp<ENCODE_SIZE && numidx<DeviceListResp->deviceNumber.value)
		{
			i=0;
			while(i<DeviceListResp->deviceAddr[temp].encodesize)
				packet->data[idax++]=DeviceListResp->deviceAddr[temp].encode[i++];
			temp++;
			numidx++;
		}	
	}
	packet->size=idax;
}

//拼接上报设备状态
void joint_ReportDeviceStatusNotif_payLoad(iiot_packet * packet, Iiotap_ReportDeviceStatusNotif_payLoad * RepDevStat)
{
	uint8_t idax,tmpIdx,tmpIddx;

	idax = packet->size;
	//设备数量加入packet
	tmpIdx = 0;
	while (tmpIdx<RepDevStat->deviceNumber.encodesize)
	{
		packet->data[idax++] = RepDevStat->deviceNumber.encode[tmpIdx++];
	}
	//设备地址编码加入packet
	tmpIdx = 0;
	while (tmpIdx < RepDevStat->deviceNumber.value)
	{
		tmpIddx = 0;
		while (tmpIddx <RepDevStat->deviceAddress[tmpIdx].encodesize)
		{
			packet->data[idax++] = RepDevStat->deviceAddress[tmpIdx].encode[tmpIddx++];
		}
		packet->data[idax++] = RepDevStat->status[tmpIdx++];
	}
	packet->size = idax;
	printf("ReportDeviceStatusNotif:\n");
	dump_mem(packet->data, packet->size);
}

//拼接数据
void joint_ReportDataNotif_body(iiot_packet * packet, Iiotap_ReportDataNotif_payLoad * ReportData)
{
	uint8_t idx = 0;
	uint8_t i = 0;
	uint16_t ii = 0;
	idx = packet->size;	
	packet->data[idx++]=ReportData->taskCode;
	packet->data[idx++] = ReportData->reportFlag;	
	//时间
	while (i < ReportData->timeStamp.encodesize)
		packet->data[idx++] = ReportData->timeStamp.encode[i++];
	i = 0;
	//数据个数
	while (i < ReportData->dataNumber.encodesize)
		packet->data[idx++] = ReportData->dataNumber.encode[i++];
	i = 0;

	while(i<ReportData->dataNumber.value)
	{
		ii = 0;
		while (ii < ReportData->data[i].tag.msgEncodsize)
		{
			packet->data[idx++] = ReportData->data[i].tag.ecodecont[ii++];
		}
		packet->data[idx++] = ReportData->data[i].type;

		ii = 0;
		while (ii < ReportData->data[i].encodeSize)
		{
			packet->data[idx++] = ReportData->data[i].valueEncode[ii++];
		}
		i++;
	}
	packet->size = idx;
	printf("DATA:\n");
	dump_mem(packet->data, packet->size);
}

//拼接批量数据
void joint_Batch_ReportDataNotif_payLoad(iiot_packet *packet, Iiotap_Batch_ReportDataNotif_payLoad *BatchRepData)
{
	uint8_t idx = 0;
	uint8_t i = 0;
	uint16_t ii = 0;
	uint16_t iii = 0;
	idx = packet->size;
	packet->data[idx++] = BatchRepData->taskCode;
	packet->data[idx++] = BatchRepData->reportFlag;
	//时间
	while (i < BatchRepData->timeStamp.encodesize)
		packet->data[idx++] = BatchRepData->timeStamp.encode[i++];
	//设备数量
	i = 0;
	while(i<BatchRepData->deviceNumber.encodesize)
		packet->data[idx++] = BatchRepData->deviceNumber.encode[i++];
	//设备内容
	i = 0;
	while (i < BatchRepData->device[i].dataNumber.value)
	{
		//设备地址
		ii = 0;
		while(ii<BatchRepData->device[i].deviceAddress.encodesize)
			packet->data[idx++] = BatchRepData->device[i].deviceAddress.encode[ii++];
		//设备相对偏移
		ii = 0;
		while (ii < BatchRepData->device[i].timeoffset.ieee_floatEncodesize)
			packet->data[idx++] = BatchRepData->device[i].timeoffset.encode[ii++];
		//设备数据数量
		ii = 0;
		while (ii < BatchRepData->device[i].dataNumber.encodesize)
			packet->data[idx++] = BatchRepData->device[i].dataNumber.encode[ii++];
		//设置设备数据项
		ii = 0;
		while (ii < BatchRepData->device[i].dataNumber.value)
		{
			//设置data中的tag
			iii = 0;
			while (iii < BatchRepData->device[i].data[ii].tag.msgEncodsize)
				packet->data[idx++] = BatchRepData->device[i].data[ii].tag.ecodecont[iii++];
			//设置data中的type
				packet->data[idx++] = BatchRepData->device[i].data[ii].type;
			//设置data中的value
			iii = 0;
			while (iii < BatchRepData->device[i].data[ii].encodeSize)
				packet->data[idx++] = BatchRepData->device[i].data[ii].valueEncode[iii++];
			ii++;
		}
		i++;
	}
	packet->size = idx;
	printf("BATCH_DATA:\n");
	dump_mem(packet->data, packet->size);
}

//响应查询消息
void GetDeviceListResp(iiot_packet * packet,Iiotap_Head* header,Iiotap_DeviceListResp_payLoad *DevResp)
{
	uint8_t i=0;
	uint8_t status[ENCODE_SIZE];
	uint32_t deviceAddr[DEVICE_NUM];
	//表示在线状态，一共表示8个机器1100 0011
	status[0]=0xC3;
	for(;i<DEVICE_NUM;i++)
			deviceAddr[i]=i+0xf0;
	assemble_DeviceListResp_payLoad(DevResp, DEVICE_NUM,status,deviceAddr);
	assemble_header(header,GETDECIVELISTRESP, DevResp->payloadlength,0x02,0x00);

	joint_header(packet,header);
	joint_DeviceListResp_body(packet,DevResp);
	printf("GetDeviceListResp:\n");
	dump_mem(packet->data,packet->size);
}

//发送注册信息
void SendRegisterReq(iiot_packet * packet, Iiotap_Head* head, Iiotap_reg_payLoad* reg_body) {
	//body,username,password
	assemble_reg_payLoad(reg_body, "testlotap", "123456");
	//header,type,paylength,msg
	assemble_header(head, REGISTERREQ, reg_body->payloadlength, MSG_SERIAL, 0x01);
	joint_header(packet, head);
	joint_reg_body(packet, reg_body);
}

//上传设备状态
void SendReportDeviceStatus(iiot_packet * packet, Iiotap_Head* header, Iiotap_ReportDeviceStatusNotif_payLoad * RepDevStat)
{
	uint8_t i = 0;	
	uint8_t status[TEST_NUM];
	uint32_t deviceAddr[TEST_NUM];
	//对一个设备0xf0进行修改,0000 0011 b3-b7 默认为0  b1-b2 00：删除  01增加  10：无增删 11：保留，为0？  b0  0：离线  1：在线
	status[0] = 0x03;
	status[1] = 0x03;
	for (; i<TEST_NUM; i++)
		deviceAddr[i] = i + 0xf0;
	assemble_ReportDeviceStatusNotif_payLoad(RepDevStat,TEST_NUM,status,deviceAddr);
	assemble_header(header, REPORTDEVOCESTATUSNOTIF, RepDevStat->payloadlength, 0x03, 0x00);
	joint_header(packet, header);
	joint_ReportDeviceStatusNotif_payLoad(packet, RepDevStat);
}


//装载数据
uint16_t SetDataInReport(DATA *data, char * tag, uint8_t type, uint8_t * value, uint16_t EncodeSize)
{
	uint8_t idx = 0;

	data->DataSize = getStrencode(&(data->tag), tag);
	data->type = type;
	data->DataSize += 1;
	while (idx < EncodeSize)
		data->valueEncode[idx] = value[idx++];
	data->DataSize += EncodeSize;
	data->encodeSize = EncodeSize;
	return data->DataSize;
}
//53 117 139 83
//35 75   8b  53

//上报数据消息
void SendReportData(iiot_packet * packet, Iiotap_Head* header, Iiotap_ReportDataNotif_payLoad *ReportData)
{
	ieee_float_union union_value;
	ieee_float value2;
	uint16_t dataNum=0;
	uint8_t idx=0;
	uint16_t EncodeSize=0;
	//填充数据	1	
	intEn testvalue;
	testvalue.int_value = 1;
	getintEncode(&testvalue);	
	uint8_t buf[50] = { 0xE7,0x94,0xB5,0xE5,0x8E,0x8B };//电压的utf-8编码
	SetDataInReport(&(ReportData->data[dataNum++]),buf,INT_IDX,testvalue.intEncode,testvalue.EncodeSize);
	
	// 填充数据 2
	union_value.ieee_float_value = 1;
	EncodeSize=getIEEE_floatencode(&union_value, &value2);
	SetDataInReport(&(ReportData->data[dataNum++]), "value", FLOAT_IDX, value2.encode, EncodeSize);

	//填充数据 3
	uint8_t value=0x23;
	SetDataInReport(&(ReportData->data[dataNum++]), "ubyte", UBYTE_IDX, &value,1);

	//普通上报数据
	assemble_ReportDataNotif_payLoad(ReportData, SIMPLE_SEND_DATA, dataNum);
	assemble_header(header, REPORTDATANOTIF,ReportData->payloadlength,0x01,0xf0);
	joint_header(packet, header);
	joint_ReportDataNotif_body(packet, ReportData);

}

//设置批量上传时设备信息
uint16_t SetDevInfo(Device*dev, uint32_t devAddress, float timeoffset) {
	ieee_float_union union_tmp;
	ieee_float value_f;
	uint32_t dataNum = 0;
	intEn testvalue;
	uint8_t value;
	//设备地址
	dev->deviceAddress.value = devAddress;
	dev->DevEncodeSize += getVintencode(&(dev->deviceAddress));
	//设备时间偏移
	union_tmp.ieee_float_value = timeoffset;
	dev->DevEncodeSize += getIEEE_floatencode(&union_tmp, &(dev->timeoffset));
	if (devAddress == 0xf0) {
		//设置data值

		testvalue.int_value = 14;
		getintEncode(&testvalue);
		uint8_t buf[50] = { 0x06, 0xe7 ,0x94 ,0xb5 ,0xe5, 0x8e, 0x8b };//电压的utf-8编码
		SetDataInReport(&(dev->data[dataNum++]), buf, INT_IDX, testvalue.intEncode, testvalue.EncodeSize);

		//设置data值
		union_tmp.ieee_float_value = 0;
		union_tmp.ieee_float_value = 5.32f;
		getIEEE_floatencode(&union_tmp, &value_f);
		dev->DevEncodeSize += SetDataInReport(&(dev->data[dataNum++]), "value", FLOAT_IDX, value_f.encode, value_f.ieee_floatEncodesize);
		
		//设置data值
		value = 0x06;
		dev->DevEncodeSize += SetDataInReport(&(dev->data[dataNum++]), "ubyte", UBYTE_IDX, &value, 1);

		
	}
	else {
		//设置data值
		union_tmp.ieee_float_value = 0;
		union_tmp.ieee_float_value = 58.32f;
		getIEEE_floatencode(&union_tmp, &value_f);
		dev->DevEncodeSize += SetDataInReport(&(dev->data[dataNum++]), "value", FLOAT_IDX, value_f.encode, value_f.ieee_floatEncodesize);
		//设置data值
		value = 0x09;
		dev->DevEncodeSize += SetDataInReport(&(dev->data[dataNum++]), "ubyte", UBYTE_IDX, &value, 1);
	}

	dev->dataNumber.value = dataNum;
	dev->DevEncodeSize += getVintencode(&(dev->dataNumber));
	return dev->DevEncodeSize;
}

//批量上报数据
void SendReportBatchData(iiot_packet * packet, Iiotap_Head* header, Iiotap_Batch_ReportDataNotif_payLoad *BatchRepData)
{
	uint16_t DevNum = 0;
	uint8_t idx = 0;
	uint32_t address = 0;
	float timeoffset = 0.0f;
 	//初始化数据/*此处可以设置while循环，因为是静态赋值，所以未使用*/
	address = 0xf0;
	timeoffset = 9.0f;
	SetDevInfo(&(BatchRepData->device[DevNum++]), address, timeoffset);
	
	address = 0xf3;
	timeoffset = 1778.0f;
	SetDevInfo(&(BatchRepData->device[DevNum++]), address, timeoffset);

	assemble_Batch_ReportDataNotif_payLoad(BatchRepData, BARCH_SEND_DATA, DevNum);
	assemble_header(header, REPORTDATANOTIF, BatchRepData->payloadlength,0x00,0x00);

	joint_header(packet, header);
	joint_Batch_ReportDataNotif_payLoad(packet, BatchRepData);

}

//上报连接消息
void SendKeepLink(iiot_packet * packet, Iiotap_Head* header) {
	assemble_header(header, PINGREQ,0, 0x02, 0x01);
	joint_header(packet, header);
}

//vint的解码函数
uint32_t getVintValue(uint8_t * vtmp, uint16_t* idx)
{
	uint32_t tmp = 0;
	uint32_t vValue = 0;
	uint8_t multiplier = 0;
	vValue = vtmp[*idx];
	tmp |= ((vValue & 0x7f) << multiplier);
	multiplier += 7;
	while ((vtmp[*idx] & 0x80) != 0)
	{
		(*idx)++;
		vValue = vtmp[*idx];
		tmp |= ((vValue & 0x7f) << multiplier);
		multiplier += 7;
		if (multiplier > 49)
		{
			printf("出错！！，vint型过长");
			exit(0);
		}
	}
	(*idx)++;
	return tmp;
}

/*
uint8_t MSG_FROM_SERVER[] = { 
0x07, 0x55 ,0x1c ,0x00 ,0x04 ,0xf0 ,0x01 ,
0x03 ,0x00 ,0xf0 ,0x01 ,0x02 ,0x06, 0xe7 ,
0x94 ,0xb5 ,0xe5 ,0x8e, 0x8b ,0x06 ,0x00 ,
0x00, 0x00 ,0x01 ,0x05 ,0x76 ,0x61 ,0x6c ,
0x75 ,0x65, 0x0a, 0x3f ,0x80 ,0x00 ,0x00 };
*/

//响应设备命令
void  GetDeviceComReq(uint8_t *buf,char* data, int *Size)
{
	Iiotap_ReportDataNotif_payLoad ReData_tmp;
	Iiotap_Head head_tmp;
	iiot_packet packet_tmp;
	packet_tmp.data = data;
	uint16_t idx = 0;
	uint16_t DevNumber = 0;
	uint8_t DevAdd[8];
	uint8_t  Vsize;
	uint16_t i = 0;
	uint16_t ii = 0;
	uint16_t l = 0;
	head_tmp.Length = buf[idx];
	idx += 2;
	head_tmp.PayLength_vint.value = getVintValue(buf, &idx);
	idx += 2;
	head_tmp.DeviceAddress_vint.value = getVintValue(buf, &idx);
	if (idx != head_tmp.Length)
		printf("头部长度的读取有错误！！");
	//分析payLoad
	idx++;
	DevNumber = buf[idx++];
	if (DevNumber == 0)
	{
		DevNumber++;
	}
	ii = 0;
	//批量处理
	while (ii < DevNumber)
	{
		DevAdd[ii++] = getVintValue(buf, &idx);
	}

	ReData_tmp.dataNumber.value = getVintValue(buf,&idx);
	getVintencode(&(ReData_tmp.dataNumber));
	i = 0;
	//其中的数据
	while (i < ReData_tmp.dataNumber.value)
	{
		l = idx;
		ReData_tmp.data[i].tag.strlen_vint.value = buf[idx];

		ii = 0;
		while (ii < ReData_tmp.data[i].tag.strlen_vint.value+1)
			ReData_tmp.data[i].tag.ecodecont[ii++] = buf[idx++];
		ReData_tmp.data[i].tag.msgEncodsize = ii;

		ReData_tmp.data[i].type = buf[idx++];
		switch (ReData_tmp.data[i].type)
		{
		case BOOLEAN_IDX:
			Vsize = BOOLEAN_SIZE;break;
		case  UBYTE_IDX:
			Vsize = UBYTE_SIZE;break;
		case BYTE_IDX:
			Vsize = BYTE_SIZE;break;
		case USHORT_IDX:
			Vsize = USHORT_SIZE;break;
		case SHORT_IDX:
			Vsize = SHORT_SIZE;break;
		case UINT_IDX:
			Vsize = UINT_SIZE;break;
		case INT_IDX:
			Vsize = INT_SIZE;break;
		case ULONG_IDX:
			Vsize = ULONG_SIZE;break;
		case LONG_IDX:
			Vsize = LONG_SIZE;break;
		case FLOAT_IDX:
			Vsize = FLOAT_SIZE;break;
		case DOUBLE_IDX:
			Vsize = DOUBLE_SIZE;break;
		case DATE_IDX:
			Vsize = DATE_SIZE;break;
		case DATETIME_IDX:
			Vsize = DATETIME_SIZE;break;
		case UBYTEF_IDX:
			Vsize = UBYTEF_SIZE;break;
		case BYTEF_IDX:
			Vsize = BYTEF_SIZE;break;
		case SHORTF_IDX:
			Vsize = SHORTF_SIZE;break;
		case UINTF_IDX:
			Vsize = UINTF_IDX;break;
		case INTF_IDX:
			Vsize = INTF_IDX;break;
		case ULONGF_IDX:
			Vsize = INTF_IDX;break;
		case LONGF_IDX:
			Vsize = LONGF_IDX;break;
		case VINT_IDX:
			Vsize = VINT_IDX;break;
		}
		
		ii = 0;
		while (ii<Vsize) {
			ReData_tmp.data[i].valueEncode[ii++] = buf[idx++];
		}
		ReData_tmp.data[i].encodeSize = Vsize;
		ReData_tmp.data[i].DataSize = idx - l;
		i++;
	}
	if ((head_tmp.PayLength_vint.value+head_tmp.Length)!=idx)
	{
		printf("长度出错了！！\n");
	}
	if (DevNumber == 1)
	{
		assemble_ReportDataNotif_payLoad(&ReData_tmp, SIMPLE_SEND_DATA, ReData_tmp.dataNumber.value);
		assemble_header(&head_tmp, REPORTDATANOTIF, ReData_tmp.payloadlength, 0x00, DevAdd[0]);
		
		joint_header(&packet_tmp, &head_tmp);
		joint_ReportDataNotif_body(&packet_tmp, &ReData_tmp);
		
	}
	else
	{

	}
	*Size = packet_tmp.size;
}



//发送到服务器
void SendtoServer(SOCKET sock, char* data, int Size) {
	uint8_t i = 0;
	char szBuffer[MAXBYTE] = { 0 };
	send(sock, data, Size, 0);


	while (i<2) {
		int res = recv(sock, szBuffer, MAXBYTE, 0);
		if (res == SOCKET_ERROR) {
			printf("SOCKET ERROR:%d\n", WSAGetLastError());
			continue;
		}
		else {
			printf("server msg:\n");
			dump_mem((uint8_t*)szBuffer, res);
			//测试
			if (  (uint8_t)MSG_FROM_SERVER[1] == 0x55)
			{
				printf("MSG_FROM_SERVER:\n");
				dump_mem(MSG_FROM_SERVER, 0x23);
				GetDeviceComReq((uint8_t*)MSG_FROM_SERVER, data, &Size);
				send(sock, data, Size, 0);
			}
			i++;
		}
	}
}