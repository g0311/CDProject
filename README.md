# 🎮 ProjectCD_Acero
![image](https://github.com/user-attachments/assets/6b0c2287-f867-4d99-968a-a0471ab36a5d)
#### ProjectCD_Acero는 AWS GameLift를 활용해 데디케이티드 서버 방식으로 작동하는 멀티플레이 FPS 게임입니다
클라우드 환경에서 높은 안정성과 확장성을 확보하여 여러 플레이어가 원활하게 접속·플레이할 수 있도록 설계되었습니다

---

## 프로젝트 소개

- ProjectCD_Acero는 Unreal Engine 5.4.4 기반의 멀티플레이 FPS 게임 프로젝트입니다
- AWS GameLift Server SDK(버전 5.2.0)를 통해 데디케이티드 서버를 배포·스케일링하며, 안정적인 멀티플레이 환경을 제공합니다
- 비즈니스 로직은 AWS Lambda로 경량화하여, 빠른 응답성과 유연성을 확보했습니다

---

## 주요 기능

| ![Session](https://github.com/user-attachments/assets/1f16cd17-a6f6-4641-9399-76d65d5362e0) | ![Stats](https://github.com/user-attachments/assets/35c10f5a-f727-4d42-b3f6-2af743b78a6b) |
|:--:|:--:|
| **세션 생성 및 빠른 참가** | **플레이어 전적 및 통계** |

| ![Bomb Mode](https://github.com/user-attachments/assets/1b974294-440a-43fa-99e9-55c5238da2da) | ![Deathmatch](https://github.com/user-attachments/assets/fd8a4480-ee3d-4df7-acf0-731c97da169c) |
|:--:|:--:|
| **폭탄 설치 모드** | **데스 매치 모드** |

- **다중 맵 지원**
- **무기·아이템 시스템**
---

## 시연

- **데모 영상**: [LINK](https://youtu.be/5wetMnLOeJQ)  

---

## 아키텍처

<p align="center">
  <img src="https://github.com/user-attachments/assets/009920d6-40a2-4626-a071-4b029640a483" alt="Architecture Diagram" width="700"/>
</p>

1. **AWS GameLift**  
   - 서버 인스턴스 배포 및 자동 스케일링 관리
   - 데디케이티드 서버 방식으로, 안정적인 멀티플레이 환경 제공
2. **AWS Lambda**  
   - 게임 내 일부 경량화된 비즈니스 로직(예: 매치결과 기록, 전적 집계 등)을 서버리스로 처리
   - 빠른 응답성과 운영 비용 절감 가능
3. **데이터베이스**  
   - DynamoDB를 활용하여 플레이어 전적, 리더보드 데이터를 저장  
   - AWS IAM 권한 정책으로 최소 권한 원칙을 적용해 보안을 강화
4. **인증 및 보안**  
   - AWS Cognito 및 커스텀 인증을 통해 플레이어 인증 처리
5. **모니터링 및 로깅**  
   - CloudWatch를 활용하여 서버 상태, 지연(latency), 오류(error) 등을 실시간 모니터링 가능
---

## 개발 환경

- **Unreal Engine**: 5.4.4 
- **Amazon GameLift Server SDK**: 5.2.0
- **IDE/편집기**: JetBrains Rider
- **버전 관리**: Git

---

## 팀원

<div align="center">



| **정석영** | **조재혁** |
| :------: | :------: |
| [<img src="https://avatars.githubusercontent.com/g0311" height=150 width=150> <br/> @g0311](https://github.com/g0311) | [<img src="https://avatars.githubusercontent.com/pangqil" height=150 width=150> <br/> @pangqil](https://github.com/pangqil) |



</div>
