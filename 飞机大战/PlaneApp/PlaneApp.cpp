#include"PlaneApp.h"
#include"../GameConfig/GameConfig.h"
#include"../GunnerBox/GunnerBox.h"
#include<time.h>
#include<string>
#include"../FoePlaneBox/FoePlaneBig.h"
#include"../FoePlaneBox/FoePlaneMid.h"
#include"../FoePlaneBox/FoePlaneSma.h"

WND_PARAM(WND_X, WND_Y, WND_GAME_WIDTH, WND_GAME_HEIGHT, WND_GAME_TITLE)  // ��Ŀ���Ըĳɶ��ֽ�
DYNAMIC_CAEATE(CPlaneApp)

#define CREATE_FOEPLANE(FORTYPE)\
	CFoePlane* pFoe = new FORTYPE;\
	pFoe->InitFoe();\
	m_foebox.m_lstFoe.push_back(pFoe);

int CPlaneApp::m_rand = 0;
CPlaneApp::CPlaneApp() {

}
CPlaneApp::~CPlaneApp() {

}
void CPlaneApp::SetTimer() {
	//�趨�����ƶ��Ķ�ʱ��
	::SetTimer(hWnd, TIMER_BACKGROUND_MOVE_TIMERID, TIMER_BACKGROUND_MOVE_INTERVAL, nullptr);
	//��ʱ��ⷽ����Ƿ���
	::SetTimer(hWnd, TIMER_PLAYER_CHECKMOVE_TIMERID, TIMER_PLAYER_CHECKMOVE_INTERVAL, nullptr);
	//�ڵ��ƶ��Ķ�ʱ��
	::SetTimer(hWnd, TIMER_GUNNER_MOVE_TIMERID, TIMER_GUNNER_MOVE_INTERVAL, nullptr);
	//�ڵ�����Ķ�ʱ��
	::SetTimer(hWnd, TIMER_GUNNER_SEND_TIMERID, TIMER_GUNNER_SEND_INTERVAL, nullptr);
	//���˴�ɻ���ʱ����
	::SetTimer(hWnd, TIMER_CREATE_FOEBIG_TIMERID, TIMER_CREATE_FOEBIG_INTERVAL, nullptr);
	//�����зɻ���ʱ����
	::SetTimer(hWnd, TIMER_CREATE_FOEMID_TIMERID, TIMER_CREATE_FOEMID_INTERVAL, nullptr);
	//����С�ɻ���ʱ����
	::SetTimer(hWnd, TIMER_CREATE_FOESMA_TIMERID, TIMER_CREATE_FOESMA_INTERVAL, nullptr);
	//���˷ɻ��ƶ��Ķ�ʱ��
	::SetTimer(hWnd, TIMER_FOESTEP_TIMERID, TIMER_FOESTEP_INTERVAL, nullptr);	
	//�����ײ�Ķ�ʱ��
	::SetTimer(hWnd, TIMER_CHECK_HIT_INTERID, TIMER_CHECK_HIT_INTERVAL, nullptr);
	//��ʱ�л���ըЧ��ͼƬ
	::SetTimer(hWnd, TIMER_CHANGE_BOOM_INTERID, TIMER_CHANGE_BOOM_INTERVAL, nullptr);
}

void CPlaneApp::On_Init() {
	srand(time(NULL));
	m_score = 0;
	::loadimage(&m_scoreBoard, "./res/cardboard.png");

	//�����ĳ�ʼ��
	m_back.InitBackGround();
	//��ҷɻ��ĳ�ʼ��
	m_player.InitPlayer();
	
	//�������ж�ʱ��
	SetTimer();
	
}

void CPlaneApp::On_Paint() {
	//ÿ���ػ��ȡ�����
	CPlaneApp::m_rand = rand();
	//��ʾ����ͼ
	m_back.ShowBackGround();
	//��ʾ��ҷɻ�
	m_player.ShowPlayer();
	//��ʾ�ڵ�
	m_gunbox.ShowAllGunner();
	//��ʾ�����������˷ɻ�
	m_foebox.ShowAllFoe();
	//��ʾ���б�ը
	m_foebox.ShowAllBoomFoe();
	//��ʾ����
	ShowScore();
}

void CPlaneApp::On_Close() {

}

void CPlaneApp::On_WM_TIMER(WPARAM wParam, LPARAM lParam) {
	switch (wParam) {
	case TIMER_BACKGROUND_MOVE_TIMERID:   //�����ƶ��Ķ�ʱ��
	{
		m_back.MoveBackGround(TIMER_BACKGROUND_MOVE_STEP);
	}
	break;
	case TIMER_PLAYER_CHECKMOVE_TIMERID:   //��ʱ��ⷽ����Ƿ���
	{
		if (GetAsyncKeyState(VK_LEFT)) {  //�ж�ĳ��������Ƿ��ǰ��µ�״̬
			m_player.MovePlayer(VK_LEFT, TIMER_PLAYER_MOVE_STEP);
		}
		if (GetAsyncKeyState(VK_RIGHT)) {
			m_player.MovePlayer(VK_RIGHT, TIMER_PLAYER_MOVE_STEP);
		}
		if (GetAsyncKeyState(VK_UP)) {
			m_player.MovePlayer(VK_UP, TIMER_PLAYER_MOVE_STEP);
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			m_player.MovePlayer(VK_DOWN, TIMER_PLAYER_MOVE_STEP);
		}
	}
	break;
	case TIMER_GUNNER_MOVE_TIMERID://�ڵ���ʱ����
	{
		//�������ڵ��ŵ��ڵ����ӵ�������
		CGunner*pGun=m_player.SendGunner();
		m_gunbox.m_lstGun.push_back(pGun); 
	}
	break;
	case TIMER_GUNNER_SEND_TIMERID://�ڵ���ʱ�ƶ�
		m_gunbox.MoveAllGunner(TIMER_GUNNER_MOVE_STEP);	
	
	break;
	case TIMER_CREATE_FOEBIG_TIMERID:
	{
		CREATE_FOEPLANE(CFoePlaneBig)
	}break;
	case TIMER_CREATE_FOEMID_TIMERID:
	{
		CREATE_FOEPLANE(CFoePlaneMid)
	}break;
	case TIMER_CREATE_FOESMA_TIMERID:
	{
		CREATE_FOEPLANE(CFoePlaneSma)
	}break;
	case TIMER_FOESTEP_TIMERID:
	{
		m_foebox.MoveAllFoe();
	}break;

	case TIMER_CHECK_HIT_INTERID:
	{
		bool isBoom = false;
		//�������е��˷ɻ�
		list<CFoePlane*>::iterator iteFoe = m_foebox.m_lstFoe.begin();
		while (iteFoe != m_foebox.m_lstFoe.end()) {
			//1.�ж��Ƿ�ײ����ҷɻ������ײ����gameover
			if (*iteFoe) {
				if ((*iteFoe)->IsHitPlayer(m_player)) {
					GameOver();
					return;
				}
			}
			//�ж��Ƿ�ײ���ڵ�  ѭ�������ڵ�
			list<CGunner*>::iterator iteGun = m_gunbox.m_lstGun.begin();
			while (iteGun != m_gunbox.m_lstGun.end()) {
				if (*iteGun) {
					//�жϵ��˷ɻ��Ƿ�ײ�����ڵ�
					if ((*iteFoe)->IsHitFoe(*iteGun)) {
						delete* iteGun;
						*iteGun = nullptr;
						//ɾ���ڵ����ӽڵ�
						iteGun=m_gunbox.m_lstGun.erase(iteGun);

						(*iteFoe)->m_blood -= DEF_GUNNER_HURT;//���˷ɻ���Ѫ
						if ((*iteFoe)->m_blood == 0) {
							m_foebox.m_lstFoeBoom.push_back(*iteFoe);//����ը�ķɻ���ӵ���ը��������

							iteFoe =m_foebox.m_lstFoe.erase(iteFoe);
							isBoom = true;
							m_score += 3;
							break;
						}
						continue;
					}
					iteGun++;
				}
			}
			if (!isBoom)	iteFoe++;
			else isBoom = false;
		}
	}break;
	case TIMER_CHANGE_BOOM_INTERID:
	{
		list<CFoePlane*>::iterator ite=m_foebox.m_lstFoeBoom.begin();
		while (ite != m_foebox.m_lstFoeBoom.end()) {
			if (*ite) {
				(*ite)->m_showid--;
				if ((*ite)->m_showid < 0) {//�ж�����С��0��Ϊ����ʾ���һ�ű�ըЧ��ͼ
					//ɾ�����˱�ը�ķɻ�
					delete* ite;
					*ite = nullptr;
					//ɾ���ڵ�
					ite = m_foebox.m_lstFoeBoom.erase(ite);
					continue;
				}
			}
			ite++;
		}
	}break;
	}
}

void CPlaneApp::GameOver() {
	//ֹͣ���ж�ʱ��
	::KillTimer(hWnd, TIMER_BACKGROUND_MOVE_TIMERID);	
	::KillTimer(hWnd, TIMER_PLAYER_CHECKMOVE_TIMERID);	
	::KillTimer(hWnd, TIMER_GUNNER_MOVE_TIMERID);	
	::KillTimer(hWnd, TIMER_GUNNER_SEND_TIMERID);	
	::KillTimer(hWnd, TIMER_CREATE_FOEBIG_TIMERID);	
	::KillTimer(hWnd, TIMER_CREATE_FOEMID_TIMERID);	
	::KillTimer(hWnd, TIMER_CREATE_FOESMA_TIMERID); 	
	::KillTimer(hWnd, TIMER_FOESTEP_TIMERID);	
	::KillTimer(hWnd, TIMER_CHECK_HIT_INTERID);

	::MessageBox(hWnd, "Game Over~~", "��ʾ",MB_OK);//������

	//����Ϣ���з���һ����Ϣ WM_CLOSE
	::PostMessage(hWnd, WM_CLOSE, 0, 0);
}
void CPlaneApp::ShowScore() {
	string strScore("����=");
	char sc[5] = { 0 };
	_itoa_s(m_score, sc, 10);
	strScore += sc;
	RECT rect = { 0,0,186,60 };
	::settextcolor(RGB(255, 0, 0));//����������ɫ
	::putimage(0, 0, &m_scoreBoard);
	::drawtext(strScore.c_str(), &rect, DT_CENTER);//��ʾ�ַ���
	
} 
void CPlaneApp::On_WM_KEYDOWN(WPARAM wparam) {
	//m_player.MovePlayer(wparam, TIMER_PLAYER_MOVE_STEP);
}


