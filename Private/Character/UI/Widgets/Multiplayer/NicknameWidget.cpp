#include "Components/TextBlock.h"
#include "Character/UI/Widgets/Multiplayer/NicknameWidget.h"

void UNicknameWidget::SetPlayerName(FText PlayerNick)
{
	PlayerNickText->SetText(PlayerNick);
}
