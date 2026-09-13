-- ============================================================================
-- STANDALONE LOCAL CAT MAGNET ENGINE
-- ============================================================================
local Players = game:GetService("Players")
local RunService = game:GetService("RunService")
local player = Players.LocalPlayer
local playerGui = player:WaitForChild("PlayerGui")

getgenv().LocalCatMagnet = {
	Active = false,
	ScriptRunning = true,
	Radius = 4.5,       -- How far the cats float from you
	OrbitSpeed = 3.0    -- How fast the ring spins
}

local config = getgenv().LocalCatMagnet
local runningTime = 0

-- Clean, Minimal HUD Window
if playerGui:FindFirstChild("CatMagnetHUD") then
	playerGui.CatMagnetHUD:Destroy()
end

local screenGui = Instance.new("ScreenGui")
screenGui.Name = "CatMagnetHUD"
screenGui.ResetOnSpawn = false
screenGui.Parent = playerGui

local frame = Instance.new("Frame")
frame.Size = UDim2.new(0, 200, 0, 95)
frame.Position = UDim2.new(1, -220, 0.4, 0)
frame.BackgroundColor3 = Color3.fromRGB(20, 20, 25)
frame.BorderSizePixel = 0
frame.Active = true
frame.Draggable = true
frame.Parent = screenGui
Instance.new("UICorner", frame).CornerRadius = UDim.new(0, 8)

local title = Instance.new("TextLabel")
title.Size = UDim2.new(1, 0, 0, 30)
title.BackgroundColor3 = Color3.fromRGB(30, 30, 38)
title.TextColor3 = Color3.fromRGB(255, 170, 0)
title.Font = Enum.Font.SourceSansBold
title.TextSize = 13
title.Text = "LOCAL CAT MAGNET"
title.Parent = frame
Instance.new("UICorner", title).CornerRadius = UDim.new(0, 8)

local toggleBtn = Instance.new("TextButton")
toggleBtn.Size = UDim2.new(1, -20, 0, 40)
toggleBtn.Position = UDim2.new(0, 10, 0, 42)
toggleBtn.BackgroundColor3 = Color3.fromRGB(40, 40, 48)
toggleBtn.TextColor3 = Color3.fromRGB(180, 180, 180)
toggleBtn.Font = Enum.Font.SourceSansBold
toggleBtn.TextSize = 14
toggleBtn.Text = "MAGNET: OFF"
toggleBtn.Parent = frame
Instance.new("UICorner", toggleBtn).CornerRadius = UDim.new(0, 6)

-- Core Dynamic Handlers
local function getValidRoot()
	local char = player.Character
	local hum = char and char:FindFirstChildOfClass("Humanoid")
	local root = char and char:FindFirstChild("HumanoidRootPart")
	if hum and root and hum.Health > 0 then return root end
	return nil
end

local function gatherWorkspaceCats()
	local collected = {}
	for _, desc in ipairs(workspace:GetDescendants()) do
		if desc:IsA("Model") and desc.Name == "Cat" then
			if desc.PrimaryPart or desc:FindFirstChildWhichIsA("BasePart", true) then
				table.insert(collected, desc)
			end
		end
	end
	return collected
-- ============================================================================
-- PIPELINE CONTINUATION: REAL-TIME CLIENT POSITIONING
-- ============================================================================

local magnetLoop
magnetConnection = RunService.PostSimulation:Connect(function(deltaTime)
	if not config.ScriptRunning then
		if magnetConnection then magnetConnection:Disconnect() end
		return
	end

	if not config.Active then return end

	local hrp = getValidRoot()
	if hrp then
		runningTime = runningTime + (deltaTime * config.OrbitSpeed)
		local currentCats = gatherWorkspaceCats()
		local total = #currentCats

		if total > 0 then
			for index, cat in ipairs(currentCats) do
				if cat and cat.Parent then
					-- Arrange cats in an even mathematical wheel pattern
					local offsetAngle = (index / total) * (math.PI * 2)
					local currentAngle = runningTime + offsetAngle

					local x = math.sin(currentAngle) * config.Radius
					local z = math.cos(currentAngle) * config.Radius

					-- Align cat models directly at waist height surrounding you
					local placementCFrame = CFrame.new(hrp.Position + Vector3.new(x, 0, z))
						* CFrame.Angles(0, currentAngle + math.PI, 0)

					pcall(function()
						-- Client-side smooth translation push
						cat:PivotTo(placementCFrame)

						-- Kill assembly velocities locally so models don't lag or drop down
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

-- Connect HUD Interface Controller Action Inputs
toggleBtn.MouseButton1Click:Connect(function()
	if not config.ScriptRunning then return end
	config.Active = not config.Active

	if config.Active then
		toggleBtn.BackgroundColor3 = Color3.fromRGB(0, 140, 90)
		toggleBtn.TextColor3 = Color3.fromRGB(255, 255, 255)
		toggleBtn.Text = "MAGNET: ACTIVE"
	else
		toggleBtn.BackgroundColor3 = Color3.fromRGB(40, 40, 48)
		toggleBtn.TextColor3 = Color3.fromRGB(180, 180, 180)
		toggleBtn.Text = "MAGNET: OFF"
	end
end)

screenGui.Destroying:Connect(function()
	config.ScriptRunning = false
	config.Active = false
	print("Cat Magnet memory layers detached successfully.")
end)

print("Standalone Cat Magnet fully compiled and deployed.")
