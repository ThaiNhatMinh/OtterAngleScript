// Sample AngelScript file with Unreal-style annotation macros.
// The MacroScriptBuilder will parse these and generate C++ binding code.
// The macros are replaced with spaces before AngelScript compilation.

// ── Enums ───────────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum EMyColor
{
    Red,
    Green,
    Blue,
    Yellow
};

UENUM()
enum class EMyFlags
{
    None    = 0,
    FlagA   = 1,
    FlagB   = 2,
    FlagC   = 4,
    FlagAll = 7,
    Flag_MAX
};

// ── Structs ─────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FMyStruct
{
    UPROPERTY(BlueprintReadOnly)
    int32 Score;

    UPROPERTY(BlueprintReadWrite)
    FString PlayerName;

    UPROPERTY()
    float Duration;

    UPROPERTY()
    bool bIsComplete;

    UFUNCTION(BlueprintPure)
    float GetDuration() const;

    UFUNCTION(BlueprintCallable)
    void Reset();
};

// ── Classes ─────────────────────────────────────────────────────────────

UCLASS(Blueprintable, BlueprintType)
class AActor extends UObject
{
    UPROPERTY(BlueprintReadOnly, Category="Health")
    float Health;

    UPROPERTY(BlueprintReadWrite, Category="Combat")
    float MaxHealth;

    UPROPERTY()
    FString DisplayName;

    UPROPERTY()
    int32 Level;

    UFUNCTION(BlueprintCallable, Category="Combat")
    void TakeDamage(float DamageAmount);

    UFUNCTION(BlueprintPure, Category="Health")
    bool IsAlive() const;

    UFUNCTION(BlueprintCallable, Category="Level")
    void SetLevel(int32 NewLevel);

    UFUNCTION(BlueprintPure, Category="Level")
    int32 GetLevel() const;

    UFUNCTION(BlueprintPure, Category="Utilities")
    FString GetDisplayName() const;

    UFUNCTION(BlueprintCallable, Category="Utilities")
    void SetDisplayName(const FString& NewName);

    UFUNCTION(BlueprintCallable, Category="Combat")
    void Heal(float Amount);
};

// ── Additional class with different features ────────────────────────────

UCLASS()
class UGameMode
{
    UPROPERTY()
    int32 MaxPlayers;

    UPROPERTY()
    FString GameName;

    UFUNCTION(BlueprintCallable, Category="Game")
    void StartGame();

    UFUNCTION(BlueprintCallable, Category="Game")
    void EndGame();

    UFUNCTION(BlueprintPure, Category="Game")
    bool IsGameRunning() const;

    UFUNCTION(BlueprintCallable, Category="Players")
    void SetMaxPlayers(int32 Count);

    UFUNCTION(BlueprintPure, Category="Players")
    int32 GetMaxPlayers() const;
};
