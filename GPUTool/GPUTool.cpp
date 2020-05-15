#include "GPUTool.h"
#include <QDebug>
#include <QVector>
#include <CPtr.h>

#include "Windows.h"
#include "DXGI.h"

using namespace std;

static const IID dxgiFactory2 =
{ 0x50c83a1c, 0xe072, 0x4c48,{ 0x87, 0xb0, 0x36, 0x30, 0xfa, 0x36, 0xa6, 0xd0 } };

string WStringToString(const wstring &wstr)
{
	string str(wstr.length(), ' ');
	copy(wstr.begin(), wstr.end(), str.begin());

	return str;
}

GPUTool::GPUTool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(onGetGPUInfo()));
	connect(ui.pushButton_1, SIGNAL(clicked()), this, SLOT(onGetGPUInfo1()));
}

void GPUTool::onGetGPUInfo()
{
	ui.textEdit->clear();

	// ��������  
	IDXGIFactory * pFactory;
	IDXGIAdapter * pAdapter;
	QVector<IDXGIAdapter*> vAdapters;            // �Կ�  

	// �Կ�������  
	int iAdapterNum = 0;
	QString infoMsg;

	// ����һ��DXGI����  
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
	if (FAILED(hr))
		return;

	// ö��������  
	while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
		vAdapters.push_back(pAdapter);
		++iAdapterNum;
	}

	// ��Ϣ���   
	ui.textEdit->append(QStringLiteral("===============��ȡ��%1���Կ�===============").arg(iAdapterNum));

	for (auto &it : vAdapters) {
		//��ȡ��Ϣ
		DXGI_ADAPTER_DESC adapterDesc;
		it->GetDesc(&adapterDesc);
		wstring wstr(adapterDesc.Description);
		std::string str = WStringToString(wstr);

		//����Կ���Ϣ
		ui.textEdit->append(QStringLiteral("ϵͳ��Ƶ�ڴ�: %1 M").arg(adapterDesc.DedicatedSystemMemory / 1024 / 1024));
		ui.textEdit->append(QStringLiteral("ר����Ƶ�ڴ�: %1 M").arg(adapterDesc.DedicatedVideoMemory / 1024 / 1024));
		ui.textEdit->append(QStringLiteral("����ϵͳ�ڴ�: %1 M").arg(adapterDesc.SharedSystemMemory / 1024 / 1024));
		ui.textEdit->append(QStringLiteral("�豸����: %1").arg(str.c_str()));
		ui.textEdit->append(QStringLiteral("�豸ID: %1").arg(adapterDesc.DeviceId));
		ui.textEdit->append(QStringLiteral("PCI ID�����汾: %1").arg(adapterDesc.Revision));
		ui.textEdit->append(QStringLiteral("��ϵͳPIC ID: %1").arg(adapterDesc.SubSysId));
		ui.textEdit->append(QStringLiteral("���̱��: %1").arg(adapterDesc.VendorId));

		// ����豸  
		IDXGIOutput * pOutput;
		QVector<IDXGIOutput*> vOutputs;

		// ����豸����  
		int iOutputNum = 0;
		while (it->EnumOutputs(iOutputNum, &pOutput) != DXGI_ERROR_NOT_FOUND) {
			vOutputs.push_back(pOutput);
			iOutputNum++;
		}

		ui.textEdit->append(QStringLiteral("-----------------------------------------"));
		ui.textEdit->append(QStringLiteral("��ȡ��%1����ʾ�豸: ").arg(iOutputNum));

		for (auto &iter : vOutputs)
		{
			// ��ȡ��ʾ�豸��Ϣ  
			DXGI_OUTPUT_DESC outputDesc;
			iter->GetDesc(&outputDesc);

			// ��ȡ�豸֧��  
			UINT uModeNum = 0;
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
			UINT flags = DXGI_ENUM_MODES_INTERLACED;

			iter->GetDisplayModeList(format, flags, &uModeNum, 0);
			DXGI_MODE_DESC * pModeDescs = new DXGI_MODE_DESC[uModeNum];
			iter->GetDisplayModeList(format, flags, &uModeNum, pModeDescs);

			ui.textEdit->append(QStringLiteral("��ʾ�豸����: %1").arg(WStringToString(outputDesc.DeviceName).c_str()));
			ui.textEdit->append(QStringLiteral("��ʾ�豸��ǰ�ֱ���: %1 * %2").arg(outputDesc.DesktopCoordinates.right - outputDesc.DesktopCoordinates.left)
			.arg(outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top));

			// ��֧�ֵķֱ�����Ϣ  
			ui.textEdit->append(QStringLiteral("�ֱ�����Ϣ: "));
			for (UINT m = 0; m < uModeNum; m++) {
				ui.textEdit->append(QStringLiteral("== �ֱ���: %1 * %2     ˢ����: %3").arg(pModeDescs[m].Width).arg(pModeDescs[m].Height)
				.arg((pModeDescs[m].RefreshRate.Numerator) / (pModeDescs[m].RefreshRate.Denominator)));
			}
		}
		ui.textEdit->append(QStringLiteral("-----------------------------------------"));

		vOutputs.clear();
	}
	vAdapters.clear();
}

void GPUTool::onGetGPUInfo1()
{
	ui.textEdit->clear();
	CPtr<IDXGIFactory1>       factory;
	CPtr<IDXGIAdapter1>       adapter;

	// ��������  
	IDXGIFactory1 * pFactory;
	IDXGIAdapter1 * pAdapter;
	QVector<IDXGIAdapter1*> vAdapters;            // �Կ�  
	IID factoryIID = dxgiFactory2;

												 // �Կ�������  
	int iAdapterNum = 0;


	// ����һ��DXGI����  
	HRESULT hr = CreateDXGIFactory1(factoryIID, (void**)factory.Assign());
	if (FAILED(hr))
		return;
    
	ui.textEdit->append("Available Video Adapters: ");
	while (factory->EnumAdapters1(iAdapterNum++, adapter.Assign()) == S_OK) {
		DXGI_ADAPTER_DESC desc;
		char name[512] = "";

		hr = adapter->GetDesc(&desc);
		if (FAILED(hr))
			continue;

		/* ignore Microsoft's 'basic' renderer' */
		if (desc.VendorId == 0x1414 && desc.DeviceId == 0x8c)
			continue;

		wstring wstr(desc.Description);
		std::string str = WStringToString(wstr);

		//����Կ���Ϣ
		ui.textEdit->append(QStringLiteral("ϵͳ��Ƶ�ڴ�: %1 M").arg(desc.DedicatedSystemMemory / 1024 / 1024));
		ui.textEdit->append(QStringLiteral("ר����Ƶ�ڴ�: %1 M").arg(desc.DedicatedVideoMemory / 1024 / 1024));
		ui.textEdit->append(QStringLiteral("����ϵͳ�ڴ�: %1 M").arg(desc.SharedSystemMemory / 1024 / 1024));
		ui.textEdit->append(QStringLiteral("�豸����: %1").arg(str.c_str()));
		ui.textEdit->append(QStringLiteral("�豸ID: %1").arg(desc.DeviceId));
		ui.textEdit->append(QStringLiteral("PCI ID�����汾: %1").arg(desc.Revision));
		ui.textEdit->append(QStringLiteral("��ϵͳPIC ID: %1").arg(desc.SubSysId));
		ui.textEdit->append(QStringLiteral("���̱��: %1").arg(desc.VendorId));
		ui.textEdit->append(QStringLiteral("-----------------------------------------"));

		// ����豸  
		IDXGIOutput * pOutput;
		QVector<IDXGIOutput*> vOutputs;
		CPtr<IDXGIOutput> output;

		// ����豸����  
		int iOutputNum = 0;
		while (pAdapter->EnumOutputs(iOutputNum++, &output) == S_OK) {
			DXGI_OUTPUT_DESC desc;
			output->GetDesc(&desc);

			RECT rect = desc.DesktopCoordinates;
			ui.textEdit->append(QString("output %1:").arg(iOutputNum));
			ui.textEdit->append(QString("pos={%1, %2}").arg(rect.left).arg(rect.top));
			ui.textEdit->append(QString("size={%1, %2}").arg(rect.right - rect.left).arg(rect.bottom - rect.top));
			ui.textEdit->append(QString("attached=%1").arg(desc.AttachedToDesktop ? "true" : "false"));
			ui.textEdit->append(QStringLiteral("=========================================="));
		}
	}
}
