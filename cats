-- ============================================================================
-- UNIFIED LOCAL CAT MAGNET ENGINE
-- ============================================================================
local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local player = Players.LocalPlayer
local playerGui = player:WaitForChild("PlayerGui")

getgenv().LocalCatMagnetConfig = {
	Active = false,
	ScriptRunning = true,
	RingRadius = 5.0,   -- Distance of the cat ring from your character
	OrbitSpeed = 2.5     -- Rotational spin velocity of the ring
}

local config = getgenv().LocalCatMagnetConfig
local clientRunningTime = 0

-- UI Dashboard Implementation
if playerGui:FindFirstChild("ClientCatHUD") then
	playerGui.ClientCatHUD:Destroy()
end

local screenGui = Instance.new("ScreenGui")
screenGui.Name = "ClientCatHUD"
screenGui.ResetOnSpawn = false
screenGui.Parent = playerGui

local mainFrame = Instance.new("Frame")
mainFrame.Size = UDim2.new(0, 220, 0, 130)
mainFrame.Position = UDim2.new(1, -240, 0.4, 0)
mainFrame.BackgroundColor3 = Color3.fromRGB(30, 25, 40)
mainFrame.BorderSizePixel = 0
mainFrame.Active = true
mainFrame.Draggable = true
mainFrame.Parent = screenGui

local frameCorner = Instance.new("UICorner")
frameCorner.CornerRadius = UDim.new(0, 8)
frameCorner.Parent = mainFrame

local titleLabel = Instance.new("TextLabel")
titleLabel.Size = UDim2.new(1, 0, 0, 35)
titleLabel.BackgroundColor3 = Color3.fromRGB(45, 35, 55)
titleLabel.TextColor3 = Color3.fromRGB(255, 220, 255)
titleLabel.TextSize = 13
titleLabel.Font = Enum.Font.SourceSansBold
titleLabel.Text = "LOCAL CAT MAGNET (DEX SPEC)"
titleLabel.Parent = mainFrame

Instance.new("UICorner", titleLabel).CornerRadius = UDim.new(0, 8)

local toggleButton = Instance.new("TextButton")
toggleButton.Size = UDim2.new(1, -20, 0, 35)
toggleButton.Position = UDim2.new(0, 10, 0, 45)
toggleButton.BackgroundColor3 = Color3.fromRGB(60, 50, 75)
toggleButton.TextColor3 = Color3.fromRGB(180, 180, 180)
toggleButton.TextSize = 14
toggleButton.Font = Enum.Font.SourceSansBold
toggleButton.Text = "Local Pull [OFF]"
toggleButton.Parent = mainFrame

Instance.new("UICorner", toggleButton).CornerRadius = UDim.new(0, 6)

toggleButton.MouseButton1Click:Connect(function()
	if not config.ScriptRunning then return end
	config.Active = not config.Active
	
	if config.Active then
		toggleButton.BackgroundColor3 = Color3.fromRGB(130, 0, 180)
		toggleButton.TextColor3 = Color3.fromRGB(255, 255, 255)
		toggleButton.Text = "Local Pull [ON]"
	else
		toggleButton.BackgroundColor3 = Color3.fromRGB(60, 50, 75)
		toggleButton.TextColor3 = Color3.fromRGB(180, 180, 180)
		toggleButton.Text = "Local Pull [OFF]"
	end
end)

local killButton = Instance.new("TextButton")
killButton.Size = UDim2.new(1, -20, 0, 30)
killButton.Position = UDim2.new(0, 10, 0, 90)
killButton.BackgroundColor3 = Color3.fromRGB(150, 40, 40)
killButton.TextColor3 = Color3.fromRGB(255, 255, 255)
killButton.TextSize = 13
killButton.Font = Enum.Font.SourceSansBold
killButton.Text = "UNINSTALL SCRIPT"
killButton.Parent = mainFrame

Instance.new("UICorner", killButton).CornerRadius = UDim.new(0, 6)

-- Execution Loop Setup
local function getValidRootPart()
	local char = player.Character
	if not char then return nil end
	local hum = char:FindFirstChildOfClass("Humanoid")
	local root = char:FindFirstChild("HumanoidRootPart")
	if hum and root and hum.Health > 0 then
		return root
	end
	return nil
end

local function getLocalWorkspaceCats()
	local localRoot = getValidRootPart()
	if not localRoot then return {} end

	local localCollectedCats = {}
	for _, target in ipairs(workspace:GetDescendants()) do
		if target:IsA("Model") and target.Name == "Cat" then
			if target:FindFirstChildWhichIsA("BasePart", true) or target.PrimaryPart then
				table.insert(localCollectedCats, target)
			end
		end
	end
	return localCollectedCats
end

local clientConnection
clientConnection = RunService.PostSimulation:Connect(function(deltaTime)
	if not config.ScriptRunning then
		if clientConnection then clientConnection:Disconnect() end
		return
	end
	
	if not config.Active then return end
	
	local hrp = getValidRootPart()
	if hrp then
		clientRunningTime = clientRunningTime + (deltaTime * config.OrbitSpeed)
		
		local currentCats = getLocalWorkspaceCats()
		local totalCatsCount = #currentCats
		
		if totalCatsCount > 0 then
			local radius = config.RingRadius
			
			for index, cat in ipairs(currentCats) do
				if cat and cat.Parent then
					local offsetAngle = (index / totalCatsCount) * (math.PI * 2)
					local activeAngle = clientRunningTime + offsetAngle
					
					local lookX = math.sin(activeAngle) * radius
					local lookZ = math.cos(activeAngle) * radius
					
					local transformationCFrame = CFrame.new(hrp.Position + Vector3.new(lookX, -0.5, lookZ))
						* CFrame.Angles(0, activeAngle + math.PI, 0)
					
					pcall(function()
						cat:PivotTo(transformationCFrame)
						
						local part = cat.PrimaryPart or cat:FindFirstChildWhichIsA("BasePart", true)
						if part then
							part.AssemblyLinearVelocity = Vector3.new(0, 0, 0)
							part.AssemblyAngularVelocity = Vector3.new(0, 0, 0)
						end
					end)
				end
			end
		end
	end
end)

killButton.MouseButton1Click:Connect(function()
	config.ScriptRunning = false
	config.Active = false
	if screenGui then screenGui:Destroy() end
	print("Local Cat script uninstalled.")
end)

print("Local Cat Magnet script fully initialized.")
